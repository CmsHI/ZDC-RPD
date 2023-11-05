void plot_weightedchargespos_pos_2d(std::string filename,int filter_num_of_peaks=1,double rel_gain1=1,double rel_gain2=1,double rel_gain4=1, double rel_gain5=1){
    ROOT::RDataFrame charge_df("main_tree",(filename+".root"));

    auto event_filter= [&filter_num_of_peaks](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(filter_num_of_peaks==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
    };

    

    auto calc_charge_weighted_pos=[&rel_gain1,&rel_gain2,&rel_gain4,&rel_gain5](double& charge1,double& charge2,double& charge3,double& charge4,double& charge5){
        return (charge1*rel_gain1*1.+charge2*rel_gain2*2.+charge3*3.+charge4*rel_gain4*4.+charge5*rel_gain5*5.)/(charge1*rel_gain1+charge2*rel_gain2+charge3+charge4*rel_gain4+charge5*rel_gain5);
    };
    
    auto histo_df=charge_df.Define("charge_weighted_pos",calc_charge_weighted_pos,{"charge1","charge2","charge3","charge4","charge5"})
                           .Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"});

    

    auto hist2d=histo_df.Histo2D({"hist2d", "Charge weighted position vs position", 51,*(histo_df.Min<double>("position")), *(histo_df.Max<double>("position")), 100u, 0, 6},"position","charge_weighted_pos");

    TF1* lin=new TF1("lin","x/[1]+[0]",110,188);
    lin->SetParameters(200,-15);
    TCanvas* c1=new TCanvas();
    hist2d->Fit("lin","R");
    hist2d->DrawClone("colz");

    //c1->SaveAs((filename+"_charge_position_plots.pdf").c_str());
}