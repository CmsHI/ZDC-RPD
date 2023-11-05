void plot_total_charge_upd1(std::string filename, int filter_num_of_peaks=1,double rel_gain1=1,double rel_gain2=1,double rel_gain4=1,double rel_gain5=1){
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

    auto new_df=charge_df.Define("total_charge",calc_tot_charge,{"charge1","charge2","charge3","charge4","charge5"})
                         .Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"})
                         .Filter("position<180. && position>118.");


    TCanvas* c1=new TCanvas();
    TF1 mygaus("mygaus","gaus(0)+gaus(3)");
    mygaus.SetParameters(5000,5700,700,100,0,300);
    auto tmp_hist=new_df.Histo1D({"Fit parameters","Total charge",300,-2000,10000},"total_charge");
    tmp_hist->Fit("mygaus");
    tmp_hist->GetXaxis()->SetTitle("Charge [fC]");
    tmp_hist->DrawClone();
}