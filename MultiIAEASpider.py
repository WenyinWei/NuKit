#!/usr/bin/python3
# -*- coding: utf-8 -*-
import requests
import csv
from bs4 import BeautifulSoup
import os
import multiprocessing

import matplotlib.pyplot as plt
import networkx as nx

def SaveContents(ulist):
    # The head of table
    tableHead = ['ParentAm','ParentA', 'ParentZ', 'Parent', 'ParentN', 'T_{1/2}', 'E_X[keV]', 'Jp', \
          'Decay', 'Q-decay', 'DaughterAm', 'DaughterA', 'DaughterZ', 'Daughter', 'DaughterN']
    with open(os.getcwd()+"/Element.csv", 'w') as f:
        writer = csv.writer(f)
        writer.writerow(tableHead)
        for i in range(len(ulist)):
            writer.writerow(ulist[i])

def SaveGraph(ulist):
    with open(os.getcwd()+"/DecayGraph.csv", 'w') as f:
        writer = csv.writer(f)
        all_element = []
        for i in ulist:
            parent_node = '\'' + i[3] + '-' + i[0] + '\''
            daughter_node = '\'' + i[13] + '-' + i[10] + '\''
            parent_node.replace(' ','')
            daughter_node.replace(' ','')
            all_element.append(parent_node)
            all_element.append(daughter_node)
        all_element = set(all_element)
        writer.writerow(all_element)
        edges = []
        for i in ulist:
            parent_node = '\'' + i[3] + '-' + i[0] + '\''
            daughter_node = '\'' + i[13] + '-' + i[10] + '\''
            parent_node.replace(' ','')
            daughter_node.replace(' ','')
            if 'α' in i[8]:
                color = '#CC333F'
            elif 'β+' in i[8]:
                color = '#EB6841'
            elif 'ec' in i[8]:
                color = '#EDC951'
            elif 'IT' in i[8]:
                color = '#7DBE3C'
            else:
                color = '#BE7D3C'
            this_edge = '['+parent_node+','+daughter_node+',{color: \''+color+'\'}],\n'
            edges.append(this_edge)
        edges = set(edges)
        f.writelines(edges)

def SearchDecayDaughter(Symbol:str, A:str, dataqueue):
    if not('m' in A):
        url = 'https://www-nds.iaea.org/relnsd/LCServlet?tbltype=NR&html5=N&qry=SYMBOL=' + Symbol + '@@@ZPN=' + A
    else:
        url = 'https://www-nds.iaea.org/relnsd/LCServlet?tbltype=NR&html5=N&qry=SYMBOL=' + Symbol + '@@@ZPN=' + A.split('m')[0]
    response = requests.get(url=url)
    # print(response.text)
    # Let BeautifulSoup handle the text
    soup = BeautifulSoup(response.text, 'html.parser')
    # You can have a neat look at the soup by the following commented line
    # print(soup.prettify())

    # The data table list
    ulist = []

    # Find all the rows of the table
    trs = soup.find_all('tr')

    tr_index = 0
    for tr in trs[2:]:
        tr_index = tr_index + 1
        # The html format would require picking the rows every three rows.
        if tr_index % 3 != 1: continue
        ui = []
        # print(tr)
        breakAtLastLoop = False
        td_index = 0;
        for td in tr:
            td_index = td_index + 1
            if breakAtLastLoop: break
            if td_index == 1 or td_index == 7:
                # print("This is parent element")
                # print(td)
                elementHead = True
                try:
                    for tdd in td.table.tr:
                        if not elementHead:
                            # '\xa0' is a blank in ISO encoding, to extract the blank, following command is used.
                            blankthrown = "".join(tdd.get_text().split())
                            if blankthrown.isdigit():
                                ui.append(int(blankthrown))
                            else:
                                ui.append(blankthrown)

                        else:
                            # For a table of element, the A and Z need to be handled specilally
                            ThisA = (str(tdd).split('<br/>')[0])
                            ThisA = (ThisA.split('>')[-1]).replace(' ', '')
                            ThisA = "".join(ThisA.split())
                            # print(ThisA, '     ', A) # for debugging
                            if ThisA == A or td_index == 7:
                                ThisA, ThisZ = str(tdd).split('<br/>')
                                # print('ThisA', ThisA)
                                # print('ThisZ', ThisZ)
                                ThisA = ThisA.split('>')[-1]
                                ThisZ = ThisZ.split('<')[0]
                                ThisA.replace(" ", "")
                                ThisA = "".join(ThisA.split())
                                ThisZ.replace(" ", "")
                                ui.append(ThisA)
                                if not ('m' in ThisA):
                                    ui.append(int(ThisA))
                                    ui.append(int(ThisZ))
                                else:
                                    ui.append(int(ThisA.split('m')[0]))
                                    ui.append(int(ThisZ))
                                elementHead = False
                            else:
                                breakAtLastLoop = True
                                break
                except:
                    breakAtLastLoop = True

            elif 2 <= td_index <= 6:
                # print(td)
                if td.get_text() != None:
                    ui.append(td.get_text())
                else:
                    ui.append('NoData')
            else:
                break
        # One row is added
        if ui != []:
            ulist.append(ui)
    dataqueue.put(ulist)
    print('I have put the daughter scheme of ' + Symbol + ' in the queue')
    return





if __name__ == '__main__':
    # lock = threading.Lock()
    AllList = []
    RequiredElements = set(['Rn_222'])
    AllElements = RequiredElements.copy()
    # RequiredElements.remove('Nonsense')
    # AllElements.remove('Nonsense')
    PickedElements = set()
    WaitElements = AllElements - PickedElements
    while not(WaitElements == set()):
        print(WaitElements)
        l = len(WaitElements)
        q = multiprocessing.Queue()  # 队列
        threads = []  # 全部线程
        PickedElements = PickedElements | WaitElements
        for i in range(l):
            # Totally l threads to do this job l个线程来执行函数
            RandomElement = WaitElements.pop()
            print(RandomElement)
            # There remains some problems with D decay which radiates H2. This daughter nucleuis does not appear in the daughter item, so bug arises.
            t = multiprocessing.Process(target=SearchDecayDaughter,
                                        args=(RandomElement.split('_')[0], RandomElement.split('_')[1], q))
            t.start()
            threads.append(t)  # Add the current thread to the [threads] list 当前线程加入全部线程中

        # 对主线程中的每一个线程都执行join()
        for thread in threads:
            thread.join()
        print('One MultiSearch has finished')

        results = []  # 保存结果
        for i in range(l):
            AllList.extend(q.get())  # 从q中拿出值，每次只能按顺序拿出一个值
        if len(AllList)==1:
            NewElement = AllList[0][13]+'_'+AllList[0][10]
            AllElements = AllElements | set([NewElement])
        else:
            AllElements = AllElements | set([i[13] + '_' + i[10] for i in AllList])
        WaitElements = AllElements - PickedElements

    print(AllElements)
    SaveContents(AllList)
    SaveGraph(AllList)


    # Abandoned graph making code
    # G = nx.Graph()
    # for i in AllList:
    #     addingNode = i[3]+'_'+i[0]
    #     if addingNode not in G.nodes(): G.add_node(addingNode, pos=(i[2],i[4]))
    #     addingNode = i[13]+'_'+i[10]
    #     if addingNode not in G.nodes(): G.add_node(addingNode, pos=(i[12],i[14]))
    # for i in AllList:
    #     parentNode = i[3] + '_' + i[0]
    #     daughterNode = i[13] + '_' + i[10]
    #     G.add_edge(parentNode, daughterNode)
    # pos = nx.get_node_attributes(G, 'pos')
    # # 把每个节点中的位置pos信息导出来
    # plt.figure(figsize=(16,16))
    # nx.draw(G, pos,
    #         with_labels=True,
    #         # node_color=range(24),
    #         node_size=200,
    #         cmap=plt.cm.Dark2,  # matplotlib的调色板，可以搜搜，很多颜色呢
    #         edge_cmap=plt.cm.Blues
    #         )
    # plt.show()
    # print(G.nodes(data=True))
    # print(G.edges(data=True))
