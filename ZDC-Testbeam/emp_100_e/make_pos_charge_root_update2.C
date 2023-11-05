void make_pos_charge_root_update2(std::string ref_file="example"){
    //C:\software\root_v6.28.04\bin\thisroot.bat

    auto ref_df= ROOT::RDF::FromCSV((ref_file+".csv").c_str(),true,';',-1,{{"position",'D'},{"run_num",'D'},{"file_name",'T'}});
    std::cout<<ref_df.Describe()<<std::endl;

    std::vector<std::string> filenames=(*(ref_df.Take<std::string>("file_name")));
    std::vector<double> positions= (*(ref_df.Take<double>("position")));
    
    TFile* output_file=new TFile((ref_file+"_charge_pos_upd2.root").c_str(),"RECREATE");
    TTree* main_tree=new TTree("main_tree","main_tree");
    TTree* fname_tree=new TTree("fname_tree","fname_tree");
    
    double pos,charge1,charge2,charge3,charge4,charge5;
    std::string fname;
    int num_peaks_ch1,num_peaks_ch2,num_peaks_ch3,num_peaks_ch4,num_peaks_ch5;

    main_tree->Branch("charge1",&charge1);
    main_tree->Branch("charge2",&charge2);
    main_tree->Branch("charge3",&charge3);
    main_tree->Branch("charge4",&charge4);
    main_tree->Branch("charge5",&charge5);
    main_tree->Branch("num_peaks_ch1",&num_peaks_ch1);
    main_tree->Branch("num_peaks_ch2",&num_peaks_ch2);
    main_tree->Branch("num_peaks_ch3",&num_peaks_ch3);
    main_tree->Branch("num_peaks_ch4",&num_peaks_ch4);
    main_tree->Branch("num_peaks_ch5",&num_peaks_ch5);
    main_tree->Branch("position",&pos);
    fname_tree->Branch("fname",&fname);


    auto fill_trees=[&main_tree,&charge1,&charge2,&charge3,&charge4,&charge5,&num_peaks_ch1,&num_peaks_ch2,&num_peaks_ch3,&num_peaks_ch4,&num_peaks_ch5](double& charge_1,double& charge_2,double& charge_3,double& charge_4,double& charge_5,int& peak1,int& peak2,int& peak3,int& peak4,int& peak5){
        charge1=charge_1;
        num_peaks_ch1=peak1;
        num_peaks_ch2=peak2;
        num_peaks_ch3=peak3;
        num_peaks_ch4=peak4;
        num_peaks_ch5=peak5;
        charge2=charge_2;
        charge3=charge_3;
        charge4=charge_4;
        charge5=charge_5;
        main_tree->Fill();
    };

    for (int i = 0; i < filenames.size(); i++)
    {
        std::cout<<filenames[i]<<std::endl;

        pos = positions[i];

        //here strats the charge calculation
        ROOT::RDataFrame ch_df("channel_tree",(filenames[i]+".root"));
        ROOT::RDataFrame info_df("info_tree",(filenames[i]+".root"));
        double freq=(*(info_df.Take<double>("frequency")))[0];
        int window=(*(info_df.Take<int>("window_size")))[0];
    
        double sum;
        double bcgint,sec_bcgint,min_bcg;
        double n;
        int up,np,n_seciter_bcg,was_peak,min_pos;

        double peak_threshold=8;

        auto signal_background=[&window,&bcgint,&n](std::vector<int>& signal){
            n=0;
            bcgint=0;
            
            for(int i=0;i<window;i++){
                
                if(i<7|| i> 30){ //7+21
                    n++;
                    bcgint+=signal[i];
                }
            }
            
            return (bcgint/n);
        };
        
        auto seconditer_bcg=[&window,&n_seciter_bcg,&sec_bcgint,&min_pos,&min_bcg,&was_peak,&peak_threshold](std::vector<int>& signal,double& firstiter_bcg){
            n_seciter_bcg=0;
            sec_bcgint=0;

            was_peak=0;
            min_pos=-1;
            min_bcg=0;

            for(int i=0;i<window;i++){
                if(TMath::Abs(signal[i]-firstiter_bcg)>peak_threshold){
                    if(TMath::Abs(signal[i]-firstiter_bcg)>min_bcg){
                        min_bcg=TMath::Abs(signal[i]-firstiter_bcg);
                        min_pos=i;
                        was_peak=1;
                    }
                }
            }

            if(was_peak==0){
                for(int i=0;i<window;i++){
                    if(TMath::Abs((double)signal[i]-firstiter_bcg)<2.5){
                        sec_bcgint+=signal[i];
                        n_seciter_bcg++;
                    }
                }
            }else if(was_peak==1){
                for(int i=0;i<window;i++){
                    if((TMath::Abs((double)signal[i]-firstiter_bcg)<2.5)&&((i<min_pos-6)||(i>min_pos+16))){
                        sec_bcgint+=signal[i];
                        n_seciter_bcg++;
                    }
                }
            }
            //cout<< n_seciter_bcg<<endl;
            return (sec_bcgint/((double)n_seciter_bcg));
        };
        
        auto signal_integral=[&window,&freq,&sum](std::vector<int>& signal,double& bcg_level){
            sum=0;
            for(int i=0;i<window;i++){
                sum+=signal[i];
            }
            
            return -((sum)-bcg_level*window)*2/4095/freq/50.*1e15;
        };

        auto num_of_peaks=[&np,&up,&window,&peak_threshold](std::vector<int>& signal,double& bcg_level){
            np=0;
            up=0;
            for(int i=0;i<window;i++){
                if(TMath::Abs(signal[i]-bcg_level)>peak_threshold){
                    if(up==0){
                        np++;
                        up=1;
                    }
                }else{
                    up=0; 
                }
            }
            return np;
        };
    
        
        auto curr_df= ch_df
                .Define("ch3_bcg",signal_background, {"ch3"})
                .Define("ch1_bcg",signal_background, {"ch1"})
                .Define("ch2_bcg",signal_background, {"ch2"})
                .Define("ch4_bcg",signal_background, {"ch4"})
                .Define("ch5_bcg",signal_background, {"ch5"})
                .Define("ch3_secbcg",seconditer_bcg, {"ch3","ch3_bcg"})
                .Define("ch1_secbcg",seconditer_bcg, {"ch1","ch1_bcg"})
                .Define("ch2_secbcg",seconditer_bcg, {"ch2","ch2_bcg"})
                .Define("ch4_secbcg",seconditer_bcg, {"ch4","ch4_bcg"})
                .Define("ch5_secbcg",seconditer_bcg, {"ch5","ch5_bcg"})
                .Define("ch3_charge",signal_integral, {"ch3","ch3_secbcg"})
                .Define("ch1_charge",signal_integral, {"ch1","ch1_secbcg"})
                .Define("ch2_charge",signal_integral, {"ch2","ch2_secbcg"})
                .Define("ch4_charge",signal_integral, {"ch4","ch4_secbcg"})
                .Define("ch5_charge",signal_integral, {"ch5","ch5_secbcg"})
                .Define("ch3_num_of_peaks",num_of_peaks, {"ch3","ch3_secbcg"})
                .Define("ch1_num_of_peaks",num_of_peaks, {"ch1","ch1_secbcg"})
                .Define("ch2_num_of_peaks",num_of_peaks, {"ch2","ch2_secbcg"})
                .Define("ch4_num_of_peaks",num_of_peaks, {"ch4","ch4_secbcg"})
                .Define("ch5_num_of_peaks",num_of_peaks, {"ch5","ch5_secbcg"});
    
        /*
                
        auto df3=ch_new_df.Filter("ch3_num_of_peaks<2 &&ch1_num_of_peaks<2 &&ch2_num_of_peaks<2 &&ch4_num_of_peaks<2 &&ch5_num_of_peaks<2 ");
        
        */

        curr_df.Foreach(fill_trees,{"ch1_charge","ch2_charge","ch3_charge","ch4_charge","ch5_charge","ch1_num_of_peaks","ch2_num_of_peaks","ch3_num_of_peaks","ch4_num_of_peaks","ch5_num_of_peaks"});
        //here ends the charge calculation
        fname=filenames[i];
        fname_tree->Fill();

        std::cout<<pos<<" OK!"<<std::endl;
    }
    

    output_file->Write();
    output_file->Close();
}