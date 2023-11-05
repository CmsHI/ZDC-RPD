void energyovercharge(const double energy=150.,std::string filename1="",std::string filename2=""){
    TFile* f1=new TFile((filename1+"1"+filename2).c_str(),"read");
    TFile* f2=new TFile((filename1+"2"+filename2).c_str(),"read");
    TFile* f3=new TFile((filename1+"3"+filename2).c_str(),"read");
    TFile* f4=new TFile((filename1+"4"+filename2).c_str(),"read");
    TFile* f5=new TFile((filename1+"5"+filename2).c_str(),"read");

    TH1F* gr1=(TH1F*)f1->Get("graph1");
    TH1F* gr2=(TH1F*)f2->Get("graph2");
    TH1F* gr3=(TH1F*)f3->Get("graph3");
    TH1F* gr4=(TH1F*)f4->Get("graph4");
    TH1F* gr5=(TH1F*)f5->Get("graph5");

    std::cout<<energy<<" "<<gr1->GetBinContent(2)<<" "<<(energy/gr1->GetBinContent(2))<<std::endl;
    std::cout<<energy<<" "<<gr2->GetBinContent(2)<<" "<<(energy/gr2->GetBinContent(2))<<std::endl;
    std::cout<<energy<<" "<<gr3->GetBinContent(2)<<" "<<(energy/gr3->GetBinContent(2))<<std::endl;
    std::cout<<energy<<" "<<gr4->GetBinContent(2)<<" "<<(energy/gr4->GetBinContent(2))<<std::endl;
    std::cout<<energy<<" "<<gr5->GetBinContent(2)<<" "<<(energy/gr5->GetBinContent(2))<<std::endl;


    std::cout<<(energy/gr1->GetBinContent(2))<<" ";
    std::cout<<(energy/gr2->GetBinContent(2))<<" ";
    std::cout<<(energy/gr3->GetBinContent(2))<<" ";
    std::cout<<(energy/gr4->GetBinContent(2))<<" ";
    std::cout<<(energy/gr5->GetBinContent(2))<<std::endl;
}