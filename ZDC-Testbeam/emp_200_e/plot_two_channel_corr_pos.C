void plot_two_channel_corr_pos(std::string filename="default",int filter_num_of_peaks=1,double xmin=97,double xmax=217,double gain1=1,double gain2=1., double gain4=1,double gain5=1){
    ROOT::RDataFrame charge_df("main_tree",(filename+".root"));
    double ch_pos_arr[5]={179.559,163.505,148.624,133.713,118.711};
    auto event_filter= [&filter_num_of_peaks](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(filter_num_of_peaks==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
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

    auto new_tmp_df=charge_df.Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"})
                        .Define("cal_charge1",ch1_plot,{"charge1"})
                        .Define("cal_charge2",ch2_plot,{"charge2"})
                        .Define("cal_charge4",ch4_plot,{"charge4"})
                        .Define("cal_charge5",ch5_plot,{"charge5"})
                        .Filter(is_not_nan_filter,{"cal_charge1","cal_charge2","charge3","cal_charge4","cal_charge5"});


    TCanvas* c1= new TCanvas();
    c1->Divide(5,5,0.01);

    TGraph* graph_arr[25];
    TLine* line_arr[125];

    for (int k=0;k<25;k++){
        graph_arr[k]=new TGraph();
    }

    int num_of_pos= (217-97)/2.5+1;
    for(int i =0;i<num_of_pos;i++){
        //correlation matrix calculation with "online" algorithm
        double pos=xmin+2.5*i;

        auto pos_filter=[&pos](double& position){
            return (TMath::Abs(position-pos)<0.01);
        };
        auto new_df =new_tmp_df.Filter(pos_filter,{"position"});

        double n=0;
        ROOT::RVec<double> v1_avg(5);
        ROOT::RVec<double> v1_avg_old(5);
        ROOT::RVec<double> v2_avg(5);
        ROOT::RVec<double> v2_avg_old(5);
        ROOT::RVec<double> M1(5);
        ROOT::RVec<double> M2(5);
        ROOT::RVec<ROOT::RVec<double>> C{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};

        ROOT::RVec<ROOT::RVec<double>> cov_matr{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};


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
        
        cout<<"";
        new_df.Foreach(calc_covmatr,{"cal_charge1","cal_charge2","charge3","cal_charge4","cal_charge5"});
        


        cov_matr=C/n;
        // a végén N osztás helyett lehetne N-1
        ROOT::RVec<double> sigma1=sqrt(M1/n);
        ROOT::RVec<double> sigma2=sqrt(M2/n);
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {   

                //corr_matr[i][j]=cov_matr[i][j]/sigma1[i]/sigma2[j];
                TGraph* gr=graph_arr[i*5+j];

                gr->SetPoint(gr->GetN(),pos,cov_matr[i][j]/sigma1[i]/sigma2[j]);
            }
            
        }
    }

    for (int k=0;k<25;k++){
        c1->cd(k+1);

        graph_arr[k]->SetMarkerStyle(kPlus);
        graph_arr[k]->SetMarkerSize(.2);
        graph_arr[k]->SetMarkerColor(kBlue);
        graph_arr[k]->GetXaxis()->SetTitle("Position [mm]");
        graph_arr[k]->GetXaxis()->SetLabelOffset(0.007);
        graph_arr[k]->Draw("AP");


        double curr_max=graph_arr[k]->GetYaxis()->GetXmax();
        double curr_min=graph_arr[k]->GetYaxis()->GetXmin();

        line_arr[k*5]=new TLine(ch_pos_arr[0],curr_max,ch_pos_arr[0],curr_min);
        line_arr[k*5+1]=new TLine(ch_pos_arr[1],curr_max,ch_pos_arr[1],curr_min);
        line_arr[k*5+2]=new TLine(ch_pos_arr[2],curr_max,ch_pos_arr[2],curr_min);
        line_arr[k*5+3]=new TLine(ch_pos_arr[3],curr_max,ch_pos_arr[3],curr_min);
        line_arr[k*5+4]=new TLine(ch_pos_arr[4],curr_max,ch_pos_arr[4],curr_min);

        line_arr[k*5]->SetLineStyle(2);
        line_arr[k*5+1]->SetLineStyle(2);
        line_arr[k*5+2]->SetLineStyle(2);
        line_arr[k*5+3]->SetLineStyle(2);
        line_arr[k*5+4]->SetLineStyle(2);

        line_arr[k*5]->SetLineColor(8);
        line_arr[k*5+1]->SetLineColor(8);
        line_arr[k*5+2]->SetLineColor(8);
        line_arr[k*5+3]->SetLineColor(8);
        line_arr[k*5+4]->SetLineColor(8);


        line_arr[k*5]->Draw("same");
        line_arr[k*5+1]->Draw("same");
        line_arr[k*5+2]->Draw("same");
        line_arr[k*5+3]->Draw("same");
        line_arr[k*5+4]->Draw("same");
        
    }    

}