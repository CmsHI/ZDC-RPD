double convolutionfunc(double *x,double* par){
    TF1 mygaus("mygaus","gaus");
    mygaus.SetParameters(par[1],par[2],par[3]);

    return mygaus.Integral(x[0]-par[0]/2.,x[0]+par[0]/2.);
}

void plot_charge_pos_2d_conv(std::string filename,int filter_num_of_peaks=1){
    ROOT::RDataFrame charge_df("main_tree",(filename+".root"));

    auto event_filter= [&filter_num_of_peaks](int& peaks1,int& peaks2,int& peaks3,int& peaks4,int& peaks5){
        if(filter_num_of_peaks==0){
            return true;
        }
        return ((peaks1<2)&&(peaks2<2)&&(peaks3<2)&&(peaks4<2)&&(peaks5<2));
    };
    
    auto histo_df=charge_df.Filter(event_filter,{"num_peaks_ch1","num_peaks_ch2","num_peaks_ch3","num_peaks_ch4","num_peaks_ch5"});

    auto hist1=histo_df.Histo2D({"hist1", "Charge [fC] of channel 1 vs position", 51,*(histo_df.Min<double>("position")), *(histo_df.Max<double>("position")), 200u, *(histo_df.Min<double>("charge1"))-500, *(histo_df.Max<double>("charge1"))+500},"position","charge1");
    auto hist2=histo_df.Histo2D({"hist2", "Charge [fC] of channel 2 vs position", 51,*(histo_df.Min<double>("position")), *(histo_df.Max<double>("position")), 200u, *(histo_df.Min<double>("charge2"))-500, *(histo_df.Max<double>("charge2"))+500},"position","charge2");
    auto hist3=histo_df.Histo2D({"hist3", "Charge [fC] of channel 3 vs position", 51,*(histo_df.Min<double>("position")), *(histo_df.Max<double>("position")), 200u, *(histo_df.Min<double>("charge3"))-500, *(histo_df.Max<double>("charge3"))+500},"position","charge3");
    auto hist4=histo_df.Histo2D({"hist4", "Charge [fC] of channel 4 vs position", 51,*(histo_df.Min<double>("position")), *(histo_df.Max<double>("position")), 200u, *(histo_df.Min<double>("charge4"))-500, *(histo_df.Max<double>("charge4"))+500},"position","charge4");
    auto hist5=histo_df.Histo2D({"hist5", "Charge [fC] of channel 5 vs position", 51,*(histo_df.Min<double>("position")), *(histo_df.Max<double>("position")), 200u, *(histo_df.Min<double>("charge5"))-500, *(histo_df.Max<double>("charge5"))+3500},"position","charge5");

    //convolution of gauss with box distribution
    
    TF1* fitfunc=new TF1("fitfunc",convolutionfunc,-63,217,4);
    fitfunc->SetParLimits(0,0,30);
    fitfunc->SetParLimits(1,0,50000);
    fitfunc->SetParLimits(2,100,217);
    fitfunc->SetParLimits(3,0,10);
    
    fitfunc->SetParNames("box width","gaus height","gaus mean","gaus sigma");
    fitfunc->SetParameters(16,400,175,4);
    //fitfunc->FixParameter(0,18);

    
    
    int w=3000;
    int h=2000;
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);

    
    TGraphErrors* posgraph=new TGraphErrors();
    

    TCanvas* c1= new TCanvas("c1","c1",w,h);
    c1->Divide(3,2,0.01);

    c1->cd(1);
    std::cout<<"Fitting channel 1";
    hist1->GetXaxis()->SetTitle("position [mm]");
    hist1->GetZaxis()->SetLabelSize(0.025);
    //hist1->GetYaxis()->SetTitle("Charge [fC]");
    //hist1->GetYaxis()->SetTitleOffset(1);
    //hist1->GetYaxis()->SetTitleColor(2);
    hist1->Fit("fitfunc");
    posgraph->SetPoint(posgraph->GetN(),posgraph->GetN()+1,(double)(fitfunc->GetParameter(2)));
    posgraph->SetPointError(posgraph->GetN()-1, 0, (double)(fitfunc->GetParError(2)));
    hist1->DrawClone("colz");
    cout<<fitfunc->Eval(fitfunc->GetParameter(2))<<endl;
    
    
    

    c1->cd(2);
    std::cout<<"Fitting channel 2";
    hist2->GetZaxis()->SetLabelSize(0.025);
    hist2->GetXaxis()->SetTitle("position [mm]");
    //hist2->GetYaxis()->SetTitle("Charge [fC]");
    hist2->Fit("fitfunc");
    posgraph->SetPoint(posgraph->GetN(),posgraph->GetN()+1,(double)(fitfunc->GetParameter(2)));
    posgraph->SetPointError(posgraph->GetN()-1, 0, (double)(fitfunc->GetParError(2)));
    hist2->DrawClone("colz");
    cout<<fitfunc->Eval(fitfunc->GetParameter(2))<<endl;

    

    c1->cd(3);
    std::cout<<"Fitting channel 3";
    hist3->GetZaxis()->SetLabelSize(0.025);
    hist3->GetXaxis()->SetTitle("position [mm]");
    //hist3->GetYaxis()->SetTitle("Charge [fC]");
    hist3->Fit("fitfunc");
    posgraph->SetPoint(posgraph->GetN(),posgraph->GetN()+1,(double)(fitfunc->GetParameter(2)));
    posgraph->SetPointError(posgraph->GetN()-1, 0, (double)(fitfunc->GetParError(2)));
    hist3->DrawClone("colz");
    cout<<fitfunc->Eval(fitfunc->GetParameter(2))<<endl;

    
    
    c1->cd(4);
    std::cout<<"Fitting channel 4";
    hist4->GetZaxis()->SetLabelSize(0.025);
    hist4->GetXaxis()->SetTitle("position [mm]");
    //hist4->GetYaxis()->SetTitle("Charge [fC]");
    hist4->Fit("fitfunc");
    posgraph->SetPoint(posgraph->GetN(),posgraph->GetN()+1,(double)(fitfunc->GetParameter(2)));
    posgraph->SetPointError(posgraph->GetN()-1, 0, (double)(fitfunc->GetParError(2)));
    hist4->DrawClone("colz");
    cout<<fitfunc->Eval(fitfunc->GetParameter(2))<<endl;

    
    
    c1->cd(5);
    std::cout<<"Fitting channel 5";
    hist5->GetZaxis()->SetLabelSize(0.025);
    hist5->GetXaxis()->SetTitle("position [mm]");
    //hist5->GetYaxis()->SetTitle("Charge [fC]");
    hist5->Fit("fitfunc");
    posgraph->SetPoint(posgraph->GetN(),posgraph->GetN()+1,(double)(fitfunc->GetParameter(2)));
    posgraph->SetPointError(posgraph->GetN()-1, 0, (double)(fitfunc->GetParError(2)));
    hist5->DrawClone("colz");
    cout<<fitfunc->Eval(fitfunc->GetParameter(2))<<endl;

    

    TCanvas* c2=new TCanvas();
    //gStyle->SetOptFit(0);

    posgraph->SetTitle("Location of channel centers");
    posgraph->SetMarkerStyle(kPlus);
    posgraph->SetMarkerSize(1.5);
    posgraph->SetMarkerColor(kBlue);
    posgraph->GetXaxis()->SetLimits(0,6);
    posgraph->GetXaxis()->SetTitle("Channel number");
    posgraph->GetYaxis()->SetTitle("Table position [mm]");
    posgraph->GetXaxis()->SetNdivisions(006);
    posgraph->Fit("pol1");
    posgraph->Draw("AP");
   

    //c1->SaveAs((filename+"_charge_position_plots.pdf").c_str());
}