void make_avg_signal_shape_root1(std::string ref_file="example",int target_channel=1){
    //C:\software\root_v6.28.04\bin\thisroot.bat

    auto ref_df= ROOT::RDF::FromCSV((ref_file+".csv").c_str(),true,';',-1,{{"position",'D'},{"run_num",'D'},{"file_name",'T'}});
    std::cout<<ref_df.Describe()<<std::endl;

    std::vector<std::string> filenames=(*(ref_df.Take<std::string>("file_name")));
    std::vector<double> positions= (*(ref_df.Take<double>("position")));
    
    TFile* output_file=new TFile((ref_file+"_signal_shape_upd2.root").c_str(),"RECREATE");
    TTree* main_tree=new TTree("main_tree","main_tree");
    TTree* fname_tree=new TTree("fname_tree","fname_tree");
    
    std::vector<double> signal1,signal2,signal3,signal4,signal5;//in ADC
    signal1={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    signal2={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    signal3={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    signal4={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    signal5={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int number_of_signals=0;

    std::string fname;
    double pos;
    int window;

    main_tree->Branch("signal1",&signal1);
    main_tree->Branch("signal2",&signal2);
    main_tree->Branch("signal3",&signal3);
    main_tree->Branch("signal4",&signal4);
    main_tree->Branch("signal5",&signal5);
    fname_tree->Branch("position",&pos);
    fname_tree->Branch("window",&window);
    fname_tree->Branch("fname",&fname);


    for (int i = 0; i < filenames.size(); i++)
    {
        std::cout<<filenames[i]<<std::endl;

        pos = positions[i];

        //here strats the charge calculation
        ROOT::RDataFrame ch_df("channel_tree",("..\\"+filenames[i]+".root"));
        ROOT::RDataFrame info_df("info_tree",("..\\"+filenames[i]+".root"));
        double freq=(*(info_df.Take<double>("frequency")))[0];
        window=(*(info_df.Take<int>("window_size")))[0];
    
        double sum;
        double bcgint,sec_bcgint,min_bcg;
        double n;
        int up,np,n_seciter_bcg,was_peak,min_pos;

        double peak_threshold=8;
        
        auto signal_background=[&window](std::vector<int>& signal){
            double n=0;
            double bcgint=0;
            
            for(int i=0;i<window;i++){
                
                if(i<7|| i> 30){ //7+21
                    n++;
                    bcgint+=signal[i];
                }
            }
            
            return (bcgint/n);
        };
        
        auto seconditer_bcg=[&window,&peak_threshold](std::vector<int>& signal,double& firstiter_bcg){
            double n_seciter_bcg=0;
            double sec_bcgint=0;

            int was_peak=0;
            int min_pos=-1;
            double min_bcg=0;

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
        

        auto num_of_peaks=[&window,&peak_threshold](std::vector<int>& signal,double& bcg_level){
            double np=0;
            double up=0;
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
    
        auto add_signals=[&window,&freq,&signal1,&signal2,&signal3,&signal4,&signal5,&peak_threshold,&number_of_signals](std::vector<int>& in_signal1,double& bcg_level1,std::vector<int>& in_signal2,double& bcg_level2,std::vector<int>& in_signal3,double& bcg_level3,std::vector<int>& in_signal4,double& bcg_level4,std::vector<int>& in_signal5,double& bcg_level5){
            int th_ind1=12;

            for(int i=0; i<window;i++){
                if(TMath::Abs(in_signal1[i]-bcg_level1)>peak_threshold){
                    th_ind1=i;
                    break;
                }
            }
            

            if((th_ind1>6)&&(th_ind1+25<window)){
                for (int i=-7;i<25;i++){
                    signal1[i+7]+=(in_signal1[i+th_ind1]-bcg_level1);
                    signal2[i+7]+=(in_signal2[i+th_ind1]-bcg_level2);
                    signal3[i+7]+=(in_signal3[i+th_ind1]-bcg_level3);
                    signal4[i+7]+=(in_signal4[i+th_ind1]-bcg_level4);
                    signal5[i+7]+=(in_signal5[i+th_ind1]-bcg_level5);
                }
                number_of_signals++;
            }
            else{ std::cout<<"strange"<<std::endl;}
            
            //std::cout<<number_of_signals<<std::endl;
        };

        auto not_ISNAN_signal_filter=[&window](ROOT::RVec<int>& in_signal1,ROOT::RVec<int>& in_signal2,ROOT::RVec<int>& in_signal3,ROOT::RVec<int>& in_signal4,ROOT::RVec<int>& in_signal5){
            for (int i = 0; i < window; i++)
            {
                if(std::isnan((double)in_signal1[i])||std::isnan((double)in_signal2[i])||std::isnan((double)in_signal3[i])||std::isnan((double)in_signal4[i])||std::isnan((double)in_signal5[i])){
                    return false;
                }
            }
            return true;
        };
        auto not_ISNAN_bcg_filter=[](double& bcg1,double& bcg2,double& bcg3,double& bcg4,double& bcg5){
            return !(std::isnan(bcg1)||std::isnan(bcg2)||std::isnan(bcg3)||std::isnan(bcg4)||std::isnan(bcg5));
        };


        auto curr_df=ch_df
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
                .Filter(not_ISNAN_bcg_filter,{"ch1_secbcg","ch2_secbcg","ch3_secbcg","ch4_secbcg","ch5_secbcg"})
                .Define("ch3_num_of_peaks",num_of_peaks, {"ch3","ch3_secbcg"})
                .Define("ch1_num_of_peaks",num_of_peaks, {"ch1","ch1_secbcg"})
                .Define("ch2_num_of_peaks",num_of_peaks, {"ch2","ch2_secbcg"})
                .Define("ch4_num_of_peaks",num_of_peaks, {"ch4","ch4_secbcg"})
                .Define("ch5_num_of_peaks",num_of_peaks, {"ch5","ch5_secbcg"})
                .Filter(not_ISNAN_signal_filter,{"ch1","ch2","ch3","ch4","ch5"})
                .Filter("(ch1_num_of_peaks<2)&&(ch2_num_of_peaks<2)&&(ch3_num_of_peaks<2)&&(ch4_num_of_peaks<2)&&(ch5_num_of_peaks<2)")
                .Filter("(ch1_num_of_peaks+ch2_num_of_peaks+ch3_num_of_peaks+ch4_num_of_peaks+ch5_num_of_peaks)>0");
        
        
        curr_df.Foreach(add_signals,{"ch1","ch1_secbcg","ch2","ch2_secbcg","ch3","ch3_secbcg","ch4","ch4_secbcg","ch5","ch5_secbcg"});
        
       

        //here ends the charge calculation
        fname=filenames[i];
        

        std::cout<<pos<<" OK!"<<std::endl;
        fname_tree->Fill();
    }

    
    for(int i=0; i<signal1.size();i++){
        signal1[i]/=number_of_signals;
        signal2[i]/=number_of_signals;
        signal3[i]/=number_of_signals;
        signal4[i]/=number_of_signals;
        signal5[i]/=number_of_signals;
    }
    for (int i = 0; i < 32; i++)
    {
        std::cout<<signal1[i]<<" "<<signal2[i]<<" "<<signal3[i]<<" "<<signal4[i]<<" "<<signal5[i]<<std::endl;
    }
    
    main_tree->Fill();

    output_file->Write();
    output_file->Close();
}