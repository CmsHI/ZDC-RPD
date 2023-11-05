double calc_rel_width(const double* x){
    ROOT::RDataFrame charge_df("main_tree",("EMPP_150Gev_e_charge_pos_upd2.root"));

    auto event_filter= [](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(1==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
    };

    double rel_gain1;
    rel_gain1=x[0];
    

    auto calc_ch1=[&rel_gain1](double& charge1){
        return (rel_gain1*charge1);
    };

    auto new_df=charge_df.Define("ch1_charge",calc_ch1,{"charge1"})
                         .Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"});


    auto ch1_hist=new_df.Histo1D({"ch1_charge","Channel 1",140,2000,5500},"ch1_charge");
    auto ch3_hist=new_df.Histo1D({"ch3","Channel 3",140,2000,5500},"charge3");

    double chi2;
    chi2=ch3_hist->Chi2Test(&(*ch1_hist),"UU P CHI2");
    return chi2;
}


void gain_calib_strat2_gain1(std::string filename="EMPP_150Gev_e_charge_pos_upd2",int filter_num_of_peaks=1){
    


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

    double variable[1] = { .60};
    

    minimum->SetFunction(f);

    // Set the free variables to be minimized !
    minimum->SetVariable(0,"rel_gain1",variable[0], step[0]);
    
    cout<<"starting minimization"<<endl;
    // do the minimization
    minimum->Minimize();

    const double *xs = minimum->X();
    std::cout << "Minimum: f(" << xs[0] << "): "
                << minimum->MinValue()  << std::endl;

    
}