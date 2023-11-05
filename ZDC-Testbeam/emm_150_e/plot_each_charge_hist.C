void plot_each_charge_hist(std::string filename, int filter_num_of_peaks=1,double rel_gain1=1,double rel_gain2=1,double rel_gain4=1,double rel_gain5=1){
    ROOT::RDataFrame charge_df("main_tree",(filename+".root"));

    auto event_filter= [&filter_num_of_peaks](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(filter_num_of_peaks==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
    };

    auto calc_tot_charge=[&rel_gain1,&rel_gain2,&rel_gain4,&rel_gain5](double& charge1,double& charge2,double& charge3,double& charge4,double& charge5){
        return (rel_gain1*charge1+rel_gain2*charge2+charge3+rel_gain4*charge4+rel_gain5*charge5);
    };

    auto ch1_plot=[&rel_gain1](double& charge){
        return rel_gain1*charge;
    };
    auto ch2_plot=[&rel_gain2](double& charge){
        return rel_gain2*charge;
    };
    auto ch4_plot=[&rel_gain4](double& charge){
        return rel_gain4*charge;
    };
    auto ch5_plot=[&rel_gain5](double& charge){
        return rel_gain5*charge;
    };
    


    auto new_df=charge_df.Define("total_charge",calc_tot_charge,{"charge1","charge2","charge3","charge4","charge5"})
                         .Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"})
                         ;


    TCanvas* c1=new TCanvas();
    THStack* hstack=new THStack("hstack","Charge distribution of all channels;Charge [fC];");
    auto ch1_hist=new_df.Define("ch1_plot",ch1_plot,{"charge1"}).Histo1D({"ch1","Channel 1",200,2500,7500},"ch1_plot");
    auto ch2_hist=new_df.Define("ch2_plot",ch2_plot,{"charge2"}).Histo1D({"ch2","Channel 2",200,2500,7500},"ch2_plot");
    auto ch3_hist=new_df.Histo1D({"ch3","Channel 3",200,2500,7500},"charge3");
    auto ch4_hist=new_df.Define("ch4_plot",ch4_plot,{"charge4"}).Histo1D({"ch4","Channel 4",200,2500,7500},"ch4_plot");
    auto ch5_hist=new_df.Define("ch5_plot",ch5_plot,{"charge5"}).Histo1D({"ch5","Channel 5",200,2500,7500},"ch5_plot");

    hstack->Add((TH1D*)&(*ch1_hist));
    ch1_hist->SetLineColor(2);

    hstack->Add((TH1D*)&(*ch2_hist));
    ch2_hist->SetLineColor(3);

    
    hstack->Add((TH1D*)&(*ch3_hist));
    ch3_hist->SetLineColor(4);

    hstack->Add((TH1D*)&(*ch4_hist));
    ch4_hist->SetLineColor(6);


    ch5_hist->SetLineColor(7);
    hstack->Add((TH1D*)&(*ch5_hist));

    hstack->DrawClone("nostack");
    c1->BuildLegend();
    
}