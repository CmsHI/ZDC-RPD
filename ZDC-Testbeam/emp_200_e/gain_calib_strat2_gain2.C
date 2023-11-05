double calc_rel_width(const double* x){
    ROOT::RDataFrame charge_df("main_tree",("EMPP_200Gev_e_charge_pos_upd2.root"));

    auto event_filter= [](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(1==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
    };

    double rel_gain2;
    rel_gain2=x[0];
    

    auto calc_ch2=[&rel_gain2](double& charge2){
        return (rel_gain2*charge2);
    };

    auto new_df=charge_df.Define("ch2_charge",calc_ch2,{"charge2"})
                         .Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"});


    auto ch2_hist=new_df.Histo1D({"ch2_charge","Channel 2",140,3000,6500},"ch2_charge"); //25 fC bins
    auto ch3_hist=new_df.Histo1D({"ch3","Channel 3",140,3000,6500},"charge3");

    double chi2;
    chi2=ch3_hist->Chi2Test(&(*ch2_hist),"UU P CHI2");
    return chi2;
}


void gain_calib_strat2_gain2(std::string filename="EMPP_200Gev_e_charge_pos_upd2",int filter_num_of_peaks=1){
    


    //here begins the minimizer part
    const char * minName = "Minuit2";
    const char *algoName = "SIMPLEX";
    int randomSeed = -1;

    ROOT::Math::Minimizer* minimum =
      ROOT::Math::Factory::CreateMinimizer(minName, algoName);
    if (!minimum) {
        std::cerr << "Error: cannot create minimizer \"" << minName
                    << "\". Maybe the required library was not built?" << std::endl;
        return;
    }
    // set tolerance , etc...
    minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    minimum->SetMaxIterations(10000);  // for GSL
    minimum->SetTolerance(0.05);
    minimum->SetPrintLevel(1);


    

    // create function wrapper for minimizer
    // a IMultiGenFunction type
    ROOT::Math::Functor f(&calc_rel_width,1);
    double step[1] = {0.005};
    // starting point

    double variable[1] = { .85};
    

    minimum->SetFunction(f);

    // Set the free variables to be minimized !
    minimum->SetVariable(0,"rel_gain2",variable[0], step[0]);
    
    cout<<"starting minimization"<<endl;
    // do the minimization
    minimum->Minimize();

    const double *xs = minimum->X();
    std::cout << "Minimum: f(" << xs[0] << "): "
                << minimum->MinValue()  << std::endl;

    
}