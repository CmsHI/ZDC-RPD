void get_avg_signal_modrange(std::string infile="out_example"){
    std::cout<<"reading file"<<std::endl;
    ROOT::RDataFrame ch_df("main_tree",(infile+".root"));
    ROOT::RDataFrame info_df("fname_tree",(infile+".root"));
    
    double freq=250000000.;
    int window=(*(info_df.Take<int>("window")))[0];
    
    auto convert_form_adc=[&freq,&window](std::vector<double>& signal_adc){
        std::cout<<"convert"<<std::endl;
        std::vector<double> out;
        for (int i = 0; i < signal_adc.size(); i++)
        {
            out.push_back(signal_adc[i]*-2/4095/freq/50.*1e15);
        }
        return out;
    };

    int n=0;

    auto def_time=[&n,&freq](){
        n++;
        return (n-1);
    };
    
    auto charge_df=ch_df.Define("ch1",convert_form_adc,{"signal1"})
                        .Define("ch2",convert_form_adc,{"signal2"})
                        .Define("ch3",convert_form_adc,{"signal3"})
                        .Define("ch4",convert_form_adc,{"signal4"})
                        .Define("ch5",convert_form_adc,{"signal5"});
    
    
    std::cout<<"graphing"<<std::endl;
    TGraph* graph1=new TGraph();
    TGraph* graph2=new TGraph();
    TGraph* graph3=new TGraph();
    TGraph* graph4=new TGraph();
    TGraph* graph5=new TGraph();

    auto fill_graphs=[&window,&freq,&graph1,&graph2,&graph3,&graph4,&graph5](std::vector<double>& ch1,std::vector<double>& ch2,std::vector<double>& ch3,std::vector<double>& ch4,std::vector<double>& ch5){
        for (int i = 0; i < ch1.size(); i++)
        {
            std::cout<<ch1[i]<<" "<<ch2[i]<<" "<<ch3[i]<<" "<<ch4[i]<<" "<<ch5[i]<<std::endl;
            graph1->SetPoint(graph1->GetN(),i/freq,(double)ch1[i]);
            graph2->SetPoint(graph2->GetN(),i/freq,(double)ch2[i]);
            graph3->SetPoint(graph3->GetN(),i/freq,(double)ch3[i]);
            graph4->SetPoint(graph4->GetN(),i/freq,(double)ch4[i]);
            graph5->SetPoint(graph5->GetN(),i/freq,(double)ch5[i]);
            
        }
        std::cout<<"ok"<<std::endl;
    };

    charge_df.Foreach(fill_graphs,{"ch1","ch2","ch3","ch4","ch5"});
    std::cout<<"graphing"<<std::endl;

    TCanvas* c1=new TCanvas();
    c1->Divide(3,2,0.008,0.008);
    std::cout<<"graphing"<<std::endl;

    c1->cd(1);
    graph1->Draw();
    graph1->SetTitle("Channel 1");
    graph1->SetMarkerStyle(52);
    graph1->SetMarkerSize(.4);
    graph1->GetYaxis()->SetTitle("Charge [fC]");
    graph1->GetXaxis()->SetTitle("time [s]");

    c1->cd(2);
    graph2->Draw();
    graph2->SetTitle("Channel 2");
    graph2->SetMarkerStyle(52);
    graph2->SetMarkerSize(.4);
    graph2->GetYaxis()->SetTitle("Charge [fC]");
    graph2->GetXaxis()->SetTitle("time [s]");
    
    c1->cd(3);
    graph3->Draw();
    graph3->SetTitle("Channel 3");
    graph3->SetMarkerStyle(52);
    graph3->SetMarkerSize(.4);
    graph3->GetYaxis()->SetTitle("Charge [fC]");
    graph3->GetXaxis()->SetTitle("time [s]");
    
    c1->cd(4);
    graph4->Draw();
    graph4->SetTitle("Channel 4");
    graph4->SetMarkerStyle(52);
    graph4->SetMarkerSize(.4);
    graph4->GetYaxis()->SetTitle("Charge [fC]");
    graph4->GetXaxis()->SetTitle("time [s]");
    
    c1->cd(5);
    graph5->Draw();
    graph5->SetTitle("Channel 5");
    graph5->SetMarkerStyle(52);
    graph5->SetMarkerSize(.4);
    graph5->GetYaxis()->SetTitle("Charge [fC]");
    graph5->GetXaxis()->SetTitle("time [s]");

    c1->SaveAs((infile+"_highres.pdf").c_str());
}