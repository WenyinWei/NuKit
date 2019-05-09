#!/usr/bin/python3
# -*- coding: utf-8 -*-
import requests
import csv
from bs4 import BeautifulSoup
import os
import re  # extract numbers from a string
import multiprocessing


def SaveContents(ulist):
    """
    This is the function that write down the data we collected in the first several lines in IAEA table.
    The head of table

        Args:
        ulist: This is what we are going to write down to the file Element.csv

        Returns:
            The function itself does not return anything. Information would be stored in the Element.csv.

        Raises:
            Unknown.
    """

    tableHead = ['ParentAm','ParentA', 'ParentZ', 'Parent', 'ParentN', 'T_{1/2}', 'E_X[keV]', 'Jp', \
          'Decay', 'Q-decay', 'DaughterAm', 'DaughterA', 'DaughterZ', 'Daughter', 'DaughterN']
    with open(os.getcwd()+"/Element.csv", 'w', encoding='UTF-8') as f:
        writer = csv.writer(f)
        writer.writerow(tableHead)
        for i in range(len(ulist)):
            writer.writerow(ulist[i])


def SaveGraph(ulist):

    # This is a function to write down the data we collected in a way that suits a javascript visualization package "springy"
    with open(os.getcwd()+"/DecayGraph.csv", 'w', encoding='UTF-8') as f:
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
            if 'α' in i[8]:    color = '#CC333F'
            elif 'β+' in i[8]: color = '#EB6841'
            elif 'ec' in i[8]: color = '#EDC951'
            elif 'IT' in i[8]: color = '#7DBE3C'
            else:              color = '#BE7D3C'
            this_edge = ' [ '+parent_node+' , '+daughter_node+' ,{color: \''+color+'\', label: \'' +i[8]+ '\' }],\n'
            edges.append(this_edge)
        edges = set(edges)
        f.writelines(edges)

def SearchDecayDaughter(Symbol:str, A:str, dataqueue):
    """
    This function search for data of the element: Symabol_A in IAEA online database and put the first several columns
    of the website table as a result of search. It will eventually be put in the dataqueue to be loaded outside of
    the funciton.

        Args:
            Symbol: symbol of the element to search for
            A: mass number of the element to search for
            dataqueue: It does not offer infomation to this function. It is used to store the infomation we collect
            to make the parallel mechanism work, without need to worry about which element would be stored first.

        Returns:
            The function itself does not return anything. Information would be stored in the argument dataqueue.

        Raises:
            Unknown.
    """
    if not('m' in A):
        url = 'https://www-nds.iaea.org/relnsd/LCServlet?tbltype=NR&html5=N&qry=SYMBOL=' + Symbol + '@@@ZPN=' + A
    else:
        url = 'https://www-nds.iaea.org/relnsd/LCServlet?tbltype=NR&html5=N&qry=SYMBOL=' + Symbol + '@@@ZPN=' + A.split('m')[0]
    response = requests.get(url=url)

    # Let BeautifulSoup handle the text
    soup = BeautifulSoup(response.text, 'html.parser')

    # The data table list
    ulist = []

    # Find all the rows of the table
    trs = soup.find_all('tr')

    tr_index = 0
    # Iterate every ROW in this table.
    # tr means a row!
    for tr in trs[2:]:
        tr_index = tr_index + 1 # row index
        ui = []
        # We think the row is not useful if that is not the row we want and set breakRowLoop to True .
        breakRowLoop = False
        td_index = 0; # element index
        # Iterate every element in this row.
        # td mean an element of a row!
        for td in tr:
            td_index = td_index + 1
            if breakRowLoop: break
            if td_index == 1 or td_index == 7:
                # These two elements are chemical element format
                try:
                    elementHead = True
                    for tdd in td.table.tr:
                        if not elementHead: # If the element if not a chemical

                            # '\xa0' is a blank in ISO encoding, to extract the blank, following command is used.
                            blankthrown = "".join(tdd.get_text().split())
                            if blankthrown.isdigit():
                                ui.append(int(blankthrown))
                            else:
                                ui.append(blankthrown)
                        else: # For a table of element, the A and Z need to be handled specilally

                            # By the re package, we can find all integers in the string quickly and avoid unnoticeable
                            #  blank encoded with strange format.
                            _, ThisA, ThisZ = re.findall(r"\d+m?\d*", str(tdd))
                            if ThisA == A or td_index == 7: # Check that it is really what we want, sometimes you
                                # searched for U_235 but U_235m1 also appears

                                ui.append(ThisA)                        # 235(m1), string
                                ui.append(int(ThisA.split('m')[0]))     # 235, integer
                                ui.append(int(ThisZ))                   # 132, integer
                                elementHead = False
                            else: # If this element is not what you want, U_235m1 rather than U_235, break the loop
                                breakRowLoop = True
                                break
                except: # If the daughter is blank which correspondence to special kinds of reactions, drop it
                    breakRowLoop = True

            elif 2 <= td_index <= 6:
                # print(td)
                if td.get_text() != None:
                    ui.append(td.get_text())
                else:
                    ui.append('NoData')
                # TODO: Now we can only handle these decays which produce the corresponding item in the daughter element
                #  position, otherwise, we can not handle. This is a bug.
                if (td_index == 5) and sum([(i in td.get_text()) for i in ['α','β','IT','ec']])==0:
                    ui = []
                    breakRowLoop = True
            else: # If the 8th element reached, break the loop
                breakRowLoop = True
        # One row's elements are added. If blank, do not continue to add a blank list.
        if ui != []:
            for i in range(len(ui)): # delete all unnoticeable blanks
                if isinstance(ui[i], str): ui[i] = "".join(ui[i].split())
            ulist.append(ui)
    dataqueue.put(ulist)
    print(Symbol + '_' + A + ': decay scheme has been put in the queue')
    return


def IAEAspider(RequiredElements:set):
    """
    The main function to collect decay graph data of Required Elements

        Args:
        RequiredElements: This is the origin elements of your required decay graph. It must be in set format.
        e.x. set(['U_235']) or set(['U_235', 'U_238'])

        Returns:
            The function itself does not return anything. Information would be stored in the Element.csv and
            DecayGraph.csv.

        Raises:
            Unknown.
    """
    AllList = []
    AllElements = RequiredElements.copy()
    PickedElements = set()
    WaitElements = AllElements - PickedElements
    while not(WaitElements == set()):
        print(str(WaitElements) + ': parallel search for decay daughter of the selected element(s) has started')
        l = len(WaitElements)
        q = multiprocessing.Queue()  # queue for data 队列
        threads = []  # list for all threads 全部线程
        PickedElements = PickedElements | WaitElements
        for i in range(l):
            # There exits l threads to do this job 一共有 l 个线程来执行函数
            RandomElement = WaitElements.pop()
            print(RandomElement + ': thread search for the element has started')
            # There remains some problems with D decay which radiates H2. This daughter nucleuis does not appear in the daughter item, so bug arises.
            t = multiprocessing.Process(target=SearchDecayDaughter,
                                        args=(RandomElement.split('_')[0], RandomElement.split('_')[1], q))
            t.start()
            threads.append(t)  # Add the current thread to the [threads] list 当前线程加入全部线程中

        # Wait for every thread to be completed 对主线程中的每一个线程都执行join()
        for thread in threads:
            thread.join()

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


if __name__ == '__main__':
    IAEAspider(set(['U_235']))