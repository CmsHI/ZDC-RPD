void plot_two_channel_corr(std::string filename="default",int filter_num_of_peaks=1, double pos=-200.,int first_ch=1,int second_ch=1,double gain1=1,double gain2=1., double gain4=1,double gain5=1){
    ROOT::RDataFrame charge_df("main_tree",(filename+".root"));

    auto event_filter= [&filter_num_of_peaks](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(filter_num_of_peaks==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
    };

    auto pos_filter=[&pos](double& position){
        return (TMath::Abs(position-pos)<0.01);
    };

    auto ch1_plot=[&gain1](double& charge){
        return gain1*charge;
    };
    auto ch2_plot=[&gain2](double& charge){
        return gain2*charge;
    };
    auto ch4_plot=[&gain4](double& charge){
        return gain4*charge;
    };
    auto ch5_plot=[&gain5](double& charge){
        return gain5*charge;
    };

    auto is_not_nan_filter=[](double& charge1,double& charge2,double& charge3,double& charge4,double& charge5){
        return !(std::isnan(charge1)||std::isnan(charge2)||std::isnan(charge3)||std::isnan(charge4)||std::isnan(charge5));
    };

    auto new_df=charge_df.Filter(pos_filter,{"position"})
                        .Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"})
                        .Define("cal_charge1",ch1_plot,{"charge1"})
                        .Define("cal_charge2",ch2_plot,{"charge2"})
                        .Define("cal_charge4",ch4_plot,{"charge4"})
                        .Define("cal_charge5",ch5_plot,{"charge5"})
                        .Filter(is_not_nan_filter,{"cal_charge1","cal_charge2","charge3","cal_charge4","cal_charge5"});

    TCanvas* c1= new TCanvas();
    std::string ch_names[5]={"cal_charge1","cal_charge2","charge3","cal_charge4","cal_charge5"};
    double first_min=*(new_df.Min<double>(ch_names[first_ch-1]))-500;
    double second_min=*(new_df.Min<double>(ch_names[second_ch-1]))-500;
    double first_max=*(new_df.Max<double>(ch_names[first_ch-1]))+500;
    double second_max=*(new_df.Max<double>(ch_names[second_ch-1]))+500;
    int first_num=TMath::Ceil((first_max-first_min)/50.);
    int second_num=TMath::Ceil((second_max-second_min)/50.);
    auto hist_2d=new_df.Histo2D({"hist_2d",("Charge of channel "+std::to_string(first_ch)+" vs charge of channel "+std::to_string(second_ch)).c_str(),first_num,first_min,first_max,second_num,second_min,second_max},ch_names[first_ch-1],ch_names[second_ch-1]);
    hist_2d->DrawClone("colz");

    //correlation matrix calculation with "online" algorithm

    double n=0;
    ROOT::RVec<double> v1_avg(5);
    ROOT::RVec<double> v1_avg_old(5);
    ROOT::RVec<double> v2_avg(5);
    ROOT::RVec<double> v2_avg_old(5);
    ROOT::RVec<double> M1(5);
    ROOT::RVec<double> M2(5);
    ROOT::RVec<ROOT::RVec<double>> C{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};

    ROOT::RVec<ROOT::RVec<double>> cov_matr{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
    ROOT::RVec<ROOT::RVec<double>> corr_matr{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
    


    auto calc_covmatr=[&n,&v1_avg,&v2_avg,&C,&M1,&M2,&v1_avg_old,&v2_avg_old](double& charge1,double& charge2,double& charge3,double& charge4,double& charge5){
        n++;
        ROOT::RVec<double> v1{charge1,charge2,charge3,charge4,charge5};
        ROOT::RVec<double> v2{charge1,charge2,charge3,charge4,charge5};
        v1_avg_old=v1_avg;
        v1_avg*=(1.-(double)1./n);
        v1_avg+=v1/(double)n;
        M1+=(v1-v1_avg)*(v1-v1_avg_old);
        

        
        for (int i =0;i<5;i++){
            C[i]+=(v1[i]-v1_avg[i])*(v2-v2_avg);
        }

        v2_avg_old=v2_avg;
        v2_avg*=(1.-1./(double)n);
        v2_avg+=v2/(double)n;
        M2+=(v2-v2_avg)*(v2-v2_avg_old);
        
    };
    
    cout<<"Hi!";
    new_df.Foreach(calc_covmatr,{"cal_charge1","cal_charge2","charge3","cal_charge4","cal_charge5"});
    cout<<"Hi!";


    cov_matr=C/n;
    // a végén N osztás helyett lehetne N-1
    TH2D* h2d=new TH2D("h2d","Covarance matrix",5,0,5,5,0,5);
    ROOT::RVec<double> sigma1=sqrt(M1/n);
    ROOT::RVec<double> sigma2=sqrt(M2/n);
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {   

            corr_matr[i][j]=cov_matr[i][j]/sigma1[i]/sigma2[j];
            h2d->SetBinContent(i+1,j+1,corr_matr[i][j]);
        }
        
    }
    cout<<"Hi!";

    TCanvas* c2=new TCanvas();
    h2d->Draw("colz text");
    cout<<(corr_matr)<<endl;
    

}