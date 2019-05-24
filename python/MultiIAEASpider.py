#!/usr/bin/python3
# -*- coding: utf-8 -*-
import requests
import csv
from bs4 import BeautifulSoup
import os
import re  # extract numbers from a string
import multiprocessing
from colorama import  Fore, Back, Style, init


def SaveContents(ulist, datadir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+'/data'):
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
          'Decay', 'Q-decay', 'DaughterAm', 'DaughterA', 'DaughterZ', 'Daughter', 'DaughterN', 'lambda[h^-1]',
                 'Partial_lambda[h^-1]']
    with open(datadir+"/Element.csv", 'w', encoding='UTF-8') as f:
        writer = csv.writer(f)
        writer.writerow(tableHead)
        for i in range(len(ulist)):
            writer.writerow(ulist[i])


def SaveGraph(ulist, datadir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+'/data',
              name = 'DecayGraph.csv', htmlwriter = False):

    # This is a function to write down the data we collected in a way that suits a javascript visualization package "springy"
    if htmlwriter:
        datadir = datadir +"/"+ name
        name = "index.html"
    if not os.path.isdir(datadir):
        os.makedirs(datadir)
    with open(datadir+"/"+name, 'w', encoding='UTF-8') as f:
        all_element = []
        for i in ulist:
            parent_node = '\'' + i[3] + '-' + i[0] + '\''
            daughter_node = '\'' + i[13] + '-' + i[10] + '\''
            parent_node.replace(' ','')
            this_node = '['+parent_node+', {xpos:'+str(i[2])+', ypos:'+str(i[4])+'}],\n'
            all_element.append(this_node)
            daughter_node.replace(' ','')
            this_node = '['+daughter_node+', {xpos:'+str(i[12])+', ypos:'+str(i[14])+'}],\n'
            all_element.append(this_node)
        all_element = set(all_element)

        if htmlwriter:
            htmlHeader = ["layout: false\n", "title: Decay Chain Graph\n", "date:\n", "---\n", "<html>\n", \
                      "<meta charset=\"UTF-8\" />\n", "<body>\n",\
                      "<script src=\"https://cdn.staticfile.org/jquery/1.3.2/jquery.min.js\"></script>\n",\
                      "<script type=\"text/javascript\" src=\"/js/springy.js\"></script>\n", \
                     "<script type=\"text/javascript\" src=\"/js/springyui.js\"></script>\n", \
                      "<script>\n", "var graph = new Springy.Graph();\n", "graph.addNodes(\n"]
            f.writelines(htmlHeader)
        f.writelines(all_element)
        if htmlwriter:
            htmlHeader = [");\n", "graph.addEdges(\n"]
            f.writelines(htmlHeader)
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
            this_edge = ' [ '+parent_node+' , '+daughter_node+' ,{color: \''+color+'\', label: \'' +i[8]+ '\'}],\n'
            edges.append(this_edge)
        edges = set(edges)
        f.writelines(edges)

        if htmlwriter:
            htmlHeader = [");\n","jQuery(function(){\n",\
                      "var springy = jQuery('#springydemo').springy({\n", \
                      "graph: graph\n",\
                      "});\n",
                      "});\n",
                      "</script>\n",\
                      "<canvas id = \"springydemo\"   width = \"1500\"  height = \"1500\" >  < /canvas >\n",\
                      "</body>\n", " </html>\n"]
            f.writelines(htmlHeader)




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
                # TODO: There are some mistakes in the database that wrong data is given, e.g., Po212.
                # TODO: An accident happens when Pb-212 energy is 0.0 + X.
                if (td_index == 3) and (float(re.findall(r'\d+\.?\d*', td.get_text())[0]) != 0):
                    ui = []
                    breakRowLoop = True
            else: # If the 8th element reached, break the loop
                breakRowLoop = True
        # One row's elements are added. If blank, do not continue to add a blank list. Similarly, if 'x' in ui[8] (
        # which is the symbol of scientific notification), it often means the partial decay path only occupied a small
        #  part.
        if (ui != []) :
            if  ('x' not in ui[8]):
                for i in range(len(ui)): # delete all unnoticeable blanks
                    if isinstance(ui[i], str): ui[i] = "".join(ui[i].split())
                if ('ms' in ui[5]):   timescale = 3600*1000
                elif ('μs' in ui[5]): timescale = 3600*1000000
                elif ('ns' in ui[5]): timescale = 3600*1000000000
                elif ('s' in ui[5]):  timescale = 3600
                elif ('y' in ui[5]):  timescale = 1/365/24
                elif ('min' in ui[5]):timescale = 60
                elif ('m' in ui[5]):  timescale = 1/24/30
                elif ('d' in ui[5]):  timescale = 1/24
                elif ('h' in ui[5]):  timescale = 1
                if('x' in ui[5]):
                    tmpnums = re.findall(r'\d+\.?\d*', ui[5])
                    halfTmain = float(tmpnums[0])
                    scinota = tmpnums[1]
                    scinota = float(scinota[2:])
                    lamb = 0.69314718055995/halfTmain/(10**(scinota))*timescale
                else:
                    halfT = re.findall(r'\d+\.?\d*', ui[5])[0]
                    halfT = float(halfT)
                    lamb = 0.69314718055995/halfT*timescale

                ui.append(lamb)
                ui.append(lamb*float(re.findall(r'\d+\.?\d*', ui[8])[0])/100)
                ulist.append(ui)
    dataqueue.put(ulist)
    print(Fore.LIGHTMAGENTA_EX + Symbol +'_' + A + Style.RESET_ALL +': decay scheme has been put in the queue')
    return


def IAEAspider(RequiredElements:set, datadir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+'/data'):
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
        print(Fore.LIGHTBLUE_EX + str(WaitElements) + Style.RESET_ALL + ': parallel search for decay daughter of the '
                                                                        'selected '
                                                          'element(s) has started')
        l = len(WaitElements)
        q = multiprocessing.Queue()  # queue for data 队列
        threads = []  # list for all threads 全部线程
        PickedElements = PickedElements | WaitElements
        for i in range(l):
            # There exits l threads to do this job 一共有 l 个线程来执行函数
            RandomElement = WaitElements.pop()
            print(Fore.CYAN + RandomElement+ Style.RESET_ALL + ': thread search for the element has started')
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

    SaveContents(AllList, datadir = datadir)
    SaveGraph(AllList, datadir = datadir, name= "-".join(RequiredElements), htmlwriter=True)
    print(Back.CYAN+Fore.BLACK + str(AllElements)+": have been collected in the data")


if __name__ == '__main__':
    s = requests.session()
    s.keep_alive = False
    requests.DEFAULT_RETRIES = 5
    init(autoreset=True)
    IAEAspider(set(['U_238']))
    IAEAspider(set(['Rn_222']))
    IAEAspider(set(['Pu_239']))
    IAEAspider(set(['Np_237']))
    IAEAspider(set(['Th_232']))
