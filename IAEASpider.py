import requests
import csv
from bs4 import BeautifulSoup
import os
import networkx
import threading
from queue import Queue

def SaveContents(ulist):
    # The head of table
    tableHead = ['ParentAm','ParentA', 'ParentZ', 'Parent', 'ParentN', 'T_{1/2}', 'E_X[keV]', 'Jp', \
          'Decay', 'Q-decay', 'DaughterAm', 'DaughterA', 'DaughterZ', 'Daughter', 'DaughterN']
    with open(os.getcwd()+"/Element.csv", 'w') as f:
        writer = csv.writer(f)
        writer.writerow(tableHead)
        for i in range(len(ulist)):
            writer.writerow(ulist[i])


def SearchDecayDaughter(Symbol:str, A:str):
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
                for tdd in td.table.tr:
                    if not elementHead:
                        ui.append(tdd.get_text())
                    else:
                        # For a table of element, the A and Z need to be handled specilally
                        ThisA = (str(tdd).split('<br/>')[0])
                        ThisA = (ThisA.split('>')[-1]).replace(' ', '')
                        print(ThisA, '     ', A)
                        if ThisA == A or td_index == 7:
                            ThisA, ThisZ = str(tdd).split('<br/>')
                            # print('ThisA', ThisA)
                            # print('ThisZ', ThisZ)
                            ThisA = ThisA.split('>')[-1]
                            ThisZ = ThisZ.split('<')[0]
                            ThisA.replace(" ", "")
                            ThisZ.replace(" ", "")
                            ui.append(ThisA)
                            if not('m' in ThisA):
                                ui.append(int(ThisA))
                                ui.append(int(ThisZ))
                            else:
                                ui.append(int(ThisA.split('m')[0]))
                                ui.append(int(ThisZ))
                            elementHead = False
                        else:
                            breakAtLastLoop = True
                            break
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
    return ulist


AllList = []

RequiredElements = {'Rn_202'}
AllElements = RequiredElements
PickedElements = set()
WaitElements = AllElements - PickedElements
while not(WaitElements == set()):
    RandomElement = (AllElements-PickedElements).pop()
    print(RandomElement)
    PickedElements.add(RandomElement)
    AllList.extend(SearchDecayDaughter(RandomElement.split('_')[0], RandomElement.split('_')[1]))
    AllElements = RequiredElements | set([i[13] + '_' + i[10] for i in AllList])
    WaitElements = AllElements - PickedElements

print(AllElements)


SaveContents(AllList)