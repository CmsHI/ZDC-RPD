#include <string>
#include <vector>
#include <iostream>
#include "TXMLEngine.h"
#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"


void splitString(std::string input,std::vector<int>& output){
    //this function splits a string by spaces
    size_t pos;

    while(!input.empty()){
        pos=input.find(" ");
        if(pos==std::string::npos){
            output.emplace_back(std::stoi(input));
            break;
        }
        output.emplace_back(std::stoi(input.substr(0,pos)));
        input=input.substr(pos+1);
    }
}

void ConvertToROOT(TXMLEngine &xml, XMLNodePointer_t node,std::string out_filename = "out_example"){
    //this function will convert the relevant data of the xml file to a root file
    
    //create the output root file
    TFile* file= new TFile((out_filename+".root").c_str(),"RECREATE");

    TTree* digitizer_tree=new TTree("info_tree","Additional information");

    int window_size;
    double freq,number_of_events,serial_num;
    std::string date,posttrigger;
    digitizer_tree->Branch("frequency",&freq);//data sampling frequency
    digitizer_tree->Branch("window_size",&window_size);//number of data points per channel for each event
    digitizer_tree->Branch("posttrigger_percentage",&posttrigger); //percentage of posttriger signal in the whole output signal (same for all channels)
    digitizer_tree->Branch("serial_number",&serial_num);//serial number of digitizer
    digitizer_tree->Branch("number_of_events",&number_of_events); //number of events in the file
    digitizer_tree->Branch("datetime_conv",&date);//time and date of conversion from xml to root
  
    //record the time and date of conversion from xml to root
    TDatime* datime=new TDatime();
    date=datime->AsString();

    TTree* tree=new TTree("channel_tree","Channel data");
    
    std::vector<int> ch5;
    std::vector<int> ch1;
    std::vector<int> ch2;
    std::vector<int> ch3;
    std::vector<int> ch4;
    double timestamp,clocktime;


    tree->Branch("ch5",&ch5);
    tree->Branch("ch1",&ch1);
    tree->Branch("ch2",&ch2);
    tree->Branch("ch3",&ch3);
    tree->Branch("ch4",&ch4);
    tree->Branch("timestamp",&timestamp);
    tree->Branch("clocktime",&clocktime);

    //iterating throughthe main node childs
    XMLNodePointer_t child = xml.GetChild(node);
    
    while (child != 0) {
        if(strcmp(xml.GetNodeName(child),"event")==0){
            //getting timestamp and clocktime data per event
            XMLAttrPointer_t event_attr_pt = xml.GetFirstAttr(child);
            while(event_attr_pt!=0){
                if(strcmp(xml.GetAttrName(event_attr_pt),"timestamp")==0){
                    timestamp=std::stod(xml.GetAttrValue(event_attr_pt));
                }else if(strcmp(xml.GetAttrName(event_attr_pt),"clocktime")==0){
                    clocktime=std::stod(xml.GetAttrValue(event_attr_pt));
                }
                
                event_attr_pt=xml.GetNextAttr(event_attr_pt);
            }



            XMLNodePointer_t event_child = xml.GetChild(child);

            while(event_child!=0){
                if(strcmp(xml.GetNodeName(event_child),"trace")==0){
                    XMLAttrPointer_t ch_num = xml.GetFirstAttr(event_child);
                    //getting all channel data from each event
                    if(strcmp(xml.GetAttrValue(ch_num),"5")==0){
                        
                        splitString(xml.GetNodeContent(event_child),ch5);
                    }
                    if(strcmp(xml.GetAttrValue(ch_num),"1")==0){
                        
                        splitString(xml.GetNodeContent(event_child),ch1);
                    }
                    if(strcmp(xml.GetAttrValue(ch_num),"2")==0){
                        
                        splitString(xml.GetNodeContent(event_child),ch2);
                    }
                    if(strcmp(xml.GetAttrValue(ch_num),"3")==0){
                        
                        splitString(xml.GetNodeContent(event_child),ch3);
                    }
                    if(strcmp(xml.GetAttrValue(ch_num),"4")==0){
                        
                        splitString(xml.GetNodeContent(event_child),ch4);
                    }
                }

                event_child=xml.GetNext(event_child);
            }

            
            tree->Fill();
            //resetting the channel data values
            ch5.clear();
            ch1.clear();
            ch2.clear();
            ch3.clear();
            ch4.clear();
        } else if(strcmp(xml.GetNodeName(child),"digitizer")==0){
            //getting serial number of digitizer
            XMLAttrPointer_t serial_num_pt = xml.GetFirstAttr(child);
            while(serial_num_pt!=0){
                if(strcmp(xml.GetAttrName(serial_num_pt),"serial")==0){
                    serial_num=std::stod(xml.GetAttrValue(serial_num_pt));
                }
                
                serial_num_pt=xml.GetNextAttr(serial_num_pt);
            }
            
            //getting sampling frequency
            XMLNodePointer_t digitizer_child = xml.GetChild(child);
            while(digitizer_child!=0){
                if(strcmp(xml.GetNodeName(digitizer_child),"frequency")==0){
                    XMLAttrPointer_t fr_pt = xml.GetFirstAttr(digitizer_child);
                    
                    freq=std::stod(xml.GetAttrValue(fr_pt));
                }

                digitizer_child=xml.GetNext(digitizer_child);
            }
        
        } else if(strcmp(xml.GetNodeName(child),"settings")==0){
            //getting posttrigger percentage and window size
            XMLNodePointer_t settings_child = xml.GetChild(child);
            while(settings_child!=0){
                if(strcmp(xml.GetNodeName(settings_child),"posttrigger")==0){
                    XMLAttrPointer_t posttrigger_pt = xml.GetFirstAttr(settings_child);
                    
                    posttrigger=xml.GetAttrValue(posttrigger_pt);
                }else if(strcmp(xml.GetNodeName(settings_child),"window")==0){
                    XMLAttrPointer_t window_size_pt = xml.GetFirstAttr(settings_child);
                    
                    window_size=std::stoi(xml.GetAttrValue(window_size_pt));
                }

                settings_child=xml.GetNext(settings_child);
            }
        
        }
        child = xml.GetNext(child);
    }

    number_of_events=(tree->GetEntries());
    digitizer_tree->Fill();


    file->Write();
    file->Close();
}


void testbeam_xml_to_root(std::string filename = "example"){
    // First create engine
    TXMLEngine xml;
    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    XMLDocPointer_t xmldoc = xml.ParseFile((filename+".xml").c_str());
    
    if (!xmldoc) {
        std::cout<< "Parse failed!"<<endl;
        return;
    }
    

    // take access to main node
    XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);

    ConvertToROOT(xml,mainnode,filename);

    
    // Release memory before exit
    xml.FreeDoc(xmldoc);
}