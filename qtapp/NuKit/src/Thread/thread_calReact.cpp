#include "thread_calReact.h"

//[ stiff_system_definition
typedef boost::numeric::ublas::vector<double> vector_type;
typedef boost::numeric::ublas::matrix<double> matrix_type;

class write_on_messagebrowser
{
public:
    size_t m_every, m_count;
    thread_calReact *m_thread;

    write_on_messagebrowser(thread_calReact *m_thread, size_t every = 10)
        : m_every(every), m_count(0), m_thread(m_thread) {}

    void operator()(const vector_type &x, double t)
    {
        if ((m_count % m_every) == 0)
        {
            m_thread->report_message("This is time:" + QString::number(t));
            m_thread->set_progress(t);
        }
        ++m_count;
    }
};

void thread_calReact::report_message(const QString message)
{
    emit sig_report_message(message);
}

void thread_calReact::set_progress(double progress)
{
    emit sig_set_progress(progress);
};

void thread_calReact::run()
{
    // m_process->setProcessChannelMode(QProcess::MergedChannels); //it prints everything , even errors, use it while debugging

    // Transition: Read Reaction Info Part

    std::ifstream inDecayFile("../data/DecayInfo.csv", std::ios::in);
    std::ifstream inNeutronFile("../data/NeutronInfo.csv", std::ios::in);
    std::string lineStr;
    std::vector<std::vector<std::string>> DecaystrArray;
    std::vector<std::vector<std::string>> NeutronstrArray;
    qDebug() << "before reading the table, it works";
    while (getline(inDecayFile, lineStr))
    { // store the csv data file as 2D table 存成二维表结构
        std::stringstream ss(lineStr);
        std::string str;
        std::vector<std::string> lineArray;
        // extracted cell data seperated by the ',' 按照逗号分隔
        while (getline(ss, str, ','))
            lineArray.push_back(str);
        if (lineArray.size() <= 1)
            continue;
        DecaystrArray.push_back(lineArray);
    }

    while (getline(inNeutronFile, lineStr))
    { // store the csv data file as 2D table 存成二维表结构
        std::stringstream ss(lineStr);
        std::string str;
        std::vector<std::string> lineArray;
        // extracted cell data seperated by the ',' 按照逗号分隔
        while (getline(ss, str, ','))
            lineArray.push_back(str);
        if (lineArray.size() <= 1)
            continue;
        NeutronstrArray.push_back(lineArray);
    }

    std::vector<Element> vElement;
    for (size_t i = 1; i < DecaystrArray.size(); i++)
    {
        Element ele1(DecaystrArray[i][3], stoi(DecaystrArray[i][1]), stoi(DecaystrArray[i][2]));
        Element ele2(DecaystrArray[i][13], stoi(DecaystrArray[i][11]), stoi(DecaystrArray[i][12]));
        vElement.push_back(ele1);
        vElement.push_back(ele2);
    }

    for (size_t i = 1; i < NeutronstrArray.size(); i++)
    {
        Element ele1(NeutronstrArray[i][3], stoi(NeutronstrArray[i][1]), stoi(NeutronstrArray[i][2]));
        Element ele2(NeutronstrArray[i][13], stoi(NeutronstrArray[i][11]), stoi(NeutronstrArray[i][12]));
        vElement.push_back(ele1);
        vElement.push_back(ele2);
    }

    // Now we clear the duplicated elements and sort the vector
    sort(vElement.begin(), vElement.end(), Element::sort_element);
    std::vector<Element>::iterator unque_it = std::unique(vElement.begin(), vElement.end());
    vElement.erase(unque_it, vElement.end());

    vector_type x(vElement.size());
    matrix_type DecayJ(vElement.size(), vElement.size());
    matrix_type NeutronJ(vElement.size(), vElement.size());
    for (size_t i = 0; i < vElement.size(); i++)
    {
        x(i) = 0;
        for (size_t j = 0; j < vElement.size(); j++)
        {
            DecayJ(i, j) = 0;
            NeutronJ(i, j) = 0;
        }
    }
    qDebug()<<"Everything works well now";
    Element::write_element_list(vElement);

    for (size_t i = 1; i < DecaystrArray.size(); i++)
    { // Fill the transition matrix with reaction constant or crossection
        // Attention, this one is for decay reaction
        Element ele1(DecaystrArray[i][3], stoi(DecaystrArray[i][1]), stoi(DecaystrArray[i][2]));
        Element ele2(DecaystrArray[i][13], stoi(DecaystrArray[i][11]), stoi(DecaystrArray[i][12]));
        size_t index1, index2;
        for (size_t j = 0; j < vElement.size(); j++)
            if (ele1 == vElement[j])
            {
                index1 = j;
                break;
            }
        for (size_t j = 0; j < vElement.size(); j++)
            if (ele2 == vElement[j])
            {
                index2 = j;
                break;
            }
        double d = atof(DecaystrArray[i][16].c_str());
        DecayJ(index1, index1) -= d;
        DecayJ(index2, index1) += d;
    }

    for (size_t i = 1; i < NeutronstrArray.size(); i++)
    { // Fill the transition matrix with reaction constant or crossection
        // Attention, this one is for decay reaction
        Element ele1(NeutronstrArray[i][3], stoi(NeutronstrArray[i][1]), stoi(NeutronstrArray[i][2]));
        Element ele2(NeutronstrArray[i][13], stoi(NeutronstrArray[i][11]), stoi(NeutronstrArray[i][12]));
        size_t index1, index2;
        for (size_t j = 0; j < vElement.size(); j++)
            if (ele1 == vElement[j])
            {
                index1 = j;
                break;
            }
        for (size_t j = 0; j < vElement.size(); j++)
            if (ele2 == vElement[j])
            {
                index2 = j;
                break;
            }
        double d = atof(NeutronstrArray[i][16].c_str());
        NeutronJ(index1, index1) -= d;
        NeutronJ(index2, index1) += d;
    }
    for (size_t i = 0; i < th_elements.size(); i++)
        for (size_t j = 0; j < vElement.size(); j++)
            if (th_elements[i] == QString::fromStdString(vElement[j].outname()))
            {
                x(j) = th_elementsDensity[i];
                break;
            };

    stiff_system stiff;
    stiff.iDecayJ = DecayJ;
    stiff.iNeutronJ = NeutronJ;
    stiff.iNeutronText = n_flux_text;
    stiff_system_jacobi stiffj;
    stiffj.iDecayJ = DecayJ;
    stiffj.iNeutronJ = NeutronJ;
    stiffj.iNeutronText = n_flux_text;
    boost::numeric::odeint::observer_collection<vector_type, double> obs;
    obs.observers().push_back(write_on_messagebrowser(this, 1));
    //    obs.observers().push_back(write_on_terminal(1));
    obs.observers().push_back(write_csv(vElement, n_flux_text, 1));
    double sumofElements = 0;
    for (size_t i = 0; i < x.size(); i++)
    {
        sumofElements += x[i];
    }

    size_t num_of_steps = boost::numeric::odeint::integrate_const(boost::numeric::odeint::make_dense_output<boost::numeric::odeint::rosenbrock4<double>>(1.0e-6, 1.0e-6),
                                                                  std::make_pair(stiff, stiffj),
                                                                  x, 0.0, th_time, th_timestep, boost::ref(obs));

    //]

    qDebug() << num_of_steps << endl;
};

thread_calReact::thread_calReact(QString n_flux_text, double inputTime, double inputTimeStep, QStringList Elements, QVector<double> ElementsDensity) : n_flux_text(n_flux_text),
                                                                                                                                                       th_time(inputTime),
                                                                                                                                                       th_timestep(inputTimeStep),
                                                                                                                                                       th_elements(Elements),
                                                                                                                                                       th_elementsDensity(ElementsDensity)
{
    qDebug() << "A new calculate reaction thread has been built.";
};
