void get_avg_signal_lowresEMP200(std::string infile="out_example"){
    std::cout<<"reading file"<<std::endl;
    ROOT::RDataFrame ch_df("main_tree",(infile+".root"));
    ROOT::RDataFrame info_df("fname_tree",(infile+".root"));
    
    double freq=250000000.0;
    
    
    auto convert_form_adc=[&freq](std::vector<double>& signal_adc){
        std::vector<double> fc_hr= signal_adc;
        double ts1=(fc_hr[0]/4.+fc_hr[1]+fc_hr[2]+fc_hr[3]+fc_hr[4]+fc_hr[5]+fc_hr[6])*((-2.)/((double)4095.)/freq/50.*1e15);
        double ts2=(fc_hr[7]+fc_hr[8]+fc_hr[9]+fc_hr[10]+fc_hr[11]+fc_hr[12]+fc_hr[13]/4.)*((-2.)/((double)4095.)/freq/50.*1e15);
        double ts3=(fc_hr[13]*3./4.+fc_hr[14]+fc_hr[15]+fc_hr[16]+fc_hr[17]+fc_hr[18]+fc_hr[19]/2.)*((-2.)/((double)4095.)/freq/50.*1e15);
        double ts4=(fc_hr[19]/2.+fc_hr[20]+fc_hr[21]+fc_hr[22]+fc_hr[23]+fc_hr[24]+fc_hr[25]*3./4.)*((-2.)/((double)4095.)/freq/50.*1e15);
        double ts5=(fc_hr[25]/4.+fc_hr[26]+fc_hr[27]+fc_hr[28]+fc_hr[29]+fc_hr[30]+fc_hr[31])*((-2.)/((double)4095.)/freq/50.*1e15);
        std::vector<double> fc_lr{ts1,ts2,ts3,ts4,ts5};
        std::cout<<std::endl;
        return fc_lr;
    };

    
    
    auto charge_df=ch_df.Define("ch_signal1",convert_form_adc,{"signal1"})
                        .Define("ch_signal2",convert_form_adc,{"signal2"})
                        .Define("ch_signal3",convert_form_adc,{"signal3"})
                        .Define("ch_signal4",convert_form_adc,{"signal4"})
                        .Define("ch_signal5",convert_form_adc,{"signal5"});
    
    
    std::cout<<"graphing"<<std::endl;
    TH1F* graph1=new TH1F("graph1","EM1;TS;Charge[fC]",5,0,5);
    TH1F* graph2=new TH1F("graph2","EM2;TS;Charge[fC]",5,0,5);
    TH1F* graph3=new TH1F("graph3","EM3;TS;Charge[fC]",5,0,5);
    TH1F* graph4=new TH1F("graph4","EM4;TS;Charge[fC]",5,0,5);
    TH1F* graph5=new TH1F("graph5","EM5;TS;Charge[fC]",5,0,5);

    auto fill_graphs=[&freq,&graph1,&graph2,&graph3,&graph4,&graph5](std::vector<double>& ch1,std::vector<double>& ch2,std::vector<double>& ch3,std::vector<double>& ch4,std::vector<double>& ch5){
        for (int i = 0; i < 5; i++)
        {
            std::cout<<ch1[i]<<" "<<ch2[i]<<" "<<ch3[i]<<" "<<ch4[i]<<" "<<ch5[i]<<std::endl;
            graph1->SetBinContent(i+1,ch1[i]*0.78);
            graph2->SetBinContent(i+1,ch2[i]*1.15);
            graph3->SetBinContent(i+1,ch3[i]*1.30);
            graph4->SetBinContent(i+1,ch4[i]*1.67);
            graph5->SetBinContent(i+1,ch5[i]*0.74);
        }
        std::cout<<"ok"<<std::endl;
    };

    charge_df.Foreach(fill_graphs,{"ch_signal1","ch_signal2","ch_signal3","ch_signal4","ch_signal5"});
    std::cout<<"graphing"<<std::endl;

    TCanvas* c1=new TCanvas();
    c1->Divide(3,2,0.008,0.008);
    std::cout<<"graphing"<<std::endl;

    c1->cd(1);
    graph1->DrawClone();
    
    c1->cd(2);
    graph2->DrawClone();
    
    
    c1->cd(3);
    
    graph3->DrawClone();
   
    
    c1->cd(4);
    
    graph4->DrawClone();
    
    
    c1->cd(5);
    
    graph5->DrawClone();
    

    c1->SaveAs((infile+"_lowres.pdf").c_str());

    TFile* out_file_h=new TFile(infile+"_hists.root","recreate");
    out_file_h->cd();

    graph1->Write();
    graph2->Write();
    graph3->Write();
    graph4->Write();
    graph5->Write();
    out_file_h->Close();
}