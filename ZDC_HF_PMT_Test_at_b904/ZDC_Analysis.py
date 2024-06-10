# -*- coding: utf-8 -*-
'''
Authors: U. Guney Tok, Sitian Qian

Description: ZDC PMTs test at B904 and gain calculation | 2024.

Script usage: "python3 ZDC_Analyzer.py 'partition'"
         e.g.  python3 ZDC_Analyzer.py ZDC
         or    python3 ZDC_Analyzer.py HF     
'''
import ROOT
import glob
import sys
import os
import numpy as np

partition = sys.argv[1] # "HF" or "ZDC"
electron_charge = 1.602e-19
ZDC_sections = {'CA3770':'ZDCP-EM1','CA3771':'ZDCP-EM2','CA3772':'ZDCP-EM3','CA3773':'ZDCP-EM4','CA3775':'ZDCP-EM5','CA3785':'ZDCP-HAD1','CA3765':'ZDCP-HAD2','CA3766':'ZDCP-HAD3','CA3767':'ZDCP-HAD4','CA3784':'ZDCM-EM1','CA3782':'ZDCM-EM2','CA3780':'ZDCM-EM3','CA3779':'ZDCM-EM4','CA3777':'ZDCM-EM5','CA3786':'ZDCM-HAD1','CA3787':'ZDCM-HAD2','CA3788':'ZDCM-HAD3','CA3789':'ZDCM-HAD4'}

def get_name(eos_path): # Root files were collected as, eg1: "ZDC_CA3767_PED_1150V.root" or eg2: "HF_CA1776_PED_1250V.root"
    root_file_name = os.path.basename(eos_path)
    infos = root_file_name.split(".")[0].split("_")
    ZDC_labels = {
        "PMT_ID": infos[1],
        "run_type": infos[2],
        "HV_Value": int(infos[3][:-1])
    }
    return ZDC_labels

# Reference: Hamamatsu PMT Handbook ( https://www.hamamatsu.com/content/dam/hamamatsu-photonics/sites/documents/99_SALES_LIBRARY/etd/PMT_handbook_v4E.pdf )
def gain(mean, width):
    return (width * width * 10e-16) / (mean * electron_charge) #10e-16 comes from femtoCoulomb (fC) unit

def fit_gaussian(histogram):
    fit = ROOT.TF1("fit","gaus")
    histogram.Fit(fit,"Q")
    mean = fit.GetParameter(1)
    sigma = fit.GetParameter(2)
    return mean, sigma

files_ZDC = glob.glob("/eos/user/u/utok/904_PMT_Tests/ZDC_PMTs/*_04_2024/*.root")
files_HF = glob.glob("/eos/user/u/utok/904_PMT_Tests/HF_PMTs/*_2024/*.root")

# Initialize records and details based on the partition
if partition == "ZDC":
    files = files_ZDC
elif partition == "HF":
    files = files_HF
else:
    print("Invalid partition. Please choose 'ZDC' or 'HF'.")
    sys.exit(1)

# Extract PMT details from the filenames
records = {file: get_name(file) for file in files}
zdc_PMT_ID = list(set(record['PMT_ID'] for record in records.values()))
zdc_run_Type = list(set(record['run_type'] for record in records.values()))
zdc_HV_Value = list(set(record['HV_Value'] for record in records.values()))

print(zdc_PMT_ID, zdc_run_Type, zdc_HV_Value)

colors = [ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kMagenta]
with open(f'gain_values_{partition}.txt', 'w') as file:
    for PMT_IDs in zdc_PMT_ID:
        print(f'For {PMT_IDs} PMT')

        # Canvas for LED and PED (HV=1100) runs
        canvas = ROOT.TCanvas(f"canvas_{PMT_IDs}", "Charge Distribution", 800, 600)
        ROOT.gStyle.SetOptStat(0)
        legend = ROOT.TLegend(0.5, 0.7, 0.9, 0.9)
        
        hist_dict_led = {}
        hist_dict_ped = {}
        #hist_ped_1100 = None
        color_index = 0
        hv_values = []
        gain_values = []

        ped_means = {}
        ped_sigmas = {}
        led_means = {}
        led_sigmas = {}
        gain_fitted = {}

        for record in records:
            if records[record]['PMT_ID'] != PMT_IDs:
                continue
            
            run_type = records[record]['run_type']
            hv_value = records[record]['HV_Value']
            
            # Open ROOT file
            ftemp = ROOT.TFile.Open(record)
            
            # Access histogram 
            hist = ftemp.Get('AllSumCh/SumCharge_FIB_CH_0_FIBER_0')

            if run_type == "PED":
                if hv_value not in hist_dict_ped:
                    hist_dict_ped[hv_value] = hist.Clone()
                    hist_dict_ped[hv_value].SetDirectory(0)
                    hist_dict_ped[hv_value].SetLineColor(ROOT.kBlack)
                    if hv_value == 1100:
                        legend.AddEntry(hist_dict_ped[1100], "PED HV = 1100", "l")
                else:
                    hist_dict_ped[hv_value].Add(hist)

            elif run_type == "LED":
                if hv_value not in hist_dict_led:
                    hist_dict_led[hv_value] = hist.Clone()
                    hist_dict_led[hv_value].SetDirectory(0)
                    hist_dict_led[hv_value].SetLineColor(colors[color_index % len(colors)])
                    color_index += 1
                else:
                    hist_dict_led[hv_value].Add(hist)

            ftemp.Close()

        for hv_value, hist_ped in hist_dict_ped.items():
            mean, sigma = fit_gaussian(hist_ped)
                
            ped_means[hv_value] = mean
            ped_sigmas[hv_value] = sigma
            #print(f"Ped Mean for {hv_value} is: {ped_means[hv_value]:.2e}, Ped Sigma is: {ped_sigmas[hv_value]:.2e}")

        for hv_value, hist_led in hist_dict_led.items():
            fit_led = ROOT.TF1("led_fit", "gaus", ped_means[hv_value] + 3 * ped_sigmas[hv_value], hist_led.GetXaxis().GetXmax())
            hist_led.Fit(fit_led, "Q")
            led_means[hv_value] = fit_led.GetParameter(1)
            led_sigmas[hv_value] = fit_led.GetParameter(2)
            gain_fitted[hv_value] = gain(led_means[hv_value], led_sigmas[hv_value])
            #print(f"LED Gain for {hv_value} is: {gain_fitted[hv_value]:.3e}")
            # Write the gain values to the text file
            if partition == "ZDC":
                file.write(f"PMT_ID: {ZDC_sections[PMT_IDs]}, HV_value: {hv_value}, Gain: {gain_fitted[hv_value]:.3e}\n")
            else:
                file.write(f"PMT_ID: {PMT_IDs}, HV_value: {hv_value}, Gain: {gain_fitted[hv_value]:.3e}\n")

        # Draw LED and PED (HV=1100) runs on the same canvas
        canvas.cd()
        first_hist = True
        for hv_value, hist in hist_dict_led.items():
            if hist:
                if first_hist:
                    hist.Draw("HIST")
                    first_hist = False
                else:
                    hist.Draw("HIST SAME")        

                hv_values.append(hv_value)
                gain_values.append(gain_fitted[hv_value])
                #print(f"Gain from array {np.array(gain_fitted[hv_value], dtype=float)}")
                legend.AddEntry(hist, f"LED HV = {hv_value}, Gain = {gain_fitted[hv_value]:.2e}", "l")
        
        if hist_dict_ped:
            if first_hist:
                hist_dict_ped[1100].Draw("HIST")
            else:
                hist_dict_ped[1100].Draw("HIST SAME")
        
        # Set axis titles and ranges
        if hist_dict_led:
            first_hist = list(hist_dict_led.values())[0]
            if partition == "ZDC":
                first_hist.SetTitle(f"Charge Distribution for {ZDC_sections[PMT_IDs]};Charge (fC);Entries")
            else:
                first_hist.SetTitle(f"Charge Distribution for {PMT_IDs};Charge (fC);Entries")
            first_hist.GetXaxis().SetRangeUser(0, 1400)
            first_hist.GetYaxis().SetRangeUser(0, 2200)
        
        if hist_dict_ped and 1100 in hist_dict_ped:
            if partition == "ZDC":
                hist_dict_ped[1100].SetTitle(f"Charge Distribution for {ZDC_sections[PMT_IDs]};Charge (fC);Entries")
            else:
                hist_dict_ped[1100].SetTitle(f"Charge Distribution for {PMT_IDs};Charge (fC);Entries")
            hist_dict_ped[1100].GetXaxis().SetRangeUser(0, 1400)
            hist_dict_ped[1100].GetYaxis().SetRangeUser(0, 2200)
        
        legend.Draw()
        canvas.SaveAs(f"Results_Gain_Charge_{partition}/Charge_Distribution_{PMT_IDs}.pdf")
        canvas.Close()

        # Create a graph for HV vs Gain
        graph = ROOT.TGraph(len(hv_values), np.array(hv_values, dtype=float), np.array(gain_values, dtype=float))
        if partition == "ZDC":
            graph.SetTitle(f"HV vs Gain for {ZDC_sections[PMT_IDs]};HV (V);Gain")
        else:
            graph.SetTitle(f"HV vs Gain for {PMT_IDs};HV (V);Gain")
        graph.SetMarkerStyle(20)
        graph.SetMarkerColor(ROOT.kBlue)
        graph.SetLineColor(ROOT.kBlue)
        
        # Create canvas for the graph
        canvas_graph = ROOT.TCanvas(f"canvas_graph_{PMT_IDs}", f"HV vs Gain for {PMT_IDs}", 800, 600)
        #canvas_graph.SetLogx()
        #canvas_graph.SetLogy()
        canvas_graph.SetGrid()

        #Draw the graph with points and lines
        graph.Draw("APL")
        
        # Fit with distribution (Power law fit) 
        # gain ~ C.HV^{k.n}  | where n is number of dynodes (8 for Hamamatsu R7525, tested PMTs)
        power_law = ROOT.TF1("power_law", "[0] * TMath::Power(x, 8*[1])", min(hv_values), max(hv_values))
        power_law.SetParameters(1, 1)  # Initial guess for the parameters

        # Perform the fit multiple times to improve fit results
        for i in range(25):
            graph.Fit(power_law, "R")

        power_law.Draw("SAME")

        # Add legend to show the fit result: k-value
        graph_legend = ROOT.TLegend(0.1, 0.7, 0.4, 0.8)
        graph_legend.AddEntry(power_law, f"k value = {power_law.GetParameter(1):.3}", "l")
        graph_legend.Draw()

        canvas_graph.SaveAs(f"Results_Gain_Charge_{partition}/HV_vs_Gain_{PMT_IDs}.pdf")