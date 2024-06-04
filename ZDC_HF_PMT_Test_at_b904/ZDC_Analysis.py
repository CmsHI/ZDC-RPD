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

def get_name(eos_path):
    root_file_name = os.path.basename(eos_path)
    infos = root_file_name.split(".")[0].split("_")
    ZDC_labels = {
        "PMT_ID": infos[1],
        "run_type": infos[2],
        "HV_Value": int(infos[3][:-1])
    }
    return ZDC_labels

def gain(mean, width):
    return (width * width * 10e-16) / (mean * electron_charge)

files_ZDC = glob.glob("/eos/user/u/utok/904_PMT_Tests/ZDC_PMTs/*_04_2024/*.root")
files_HF = glob.glob("/eos/user/u/utok/904_PMT_Tests/HF_PMTs/*_2024/*.root")

if partition == "ZDC":
    records = { file: get_name(file) for file in files_ZDC}
    zdc_PMT_ID = [get_name(record)['PMT_ID'] for record in files_ZDC]
    zdc_run_Type = [get_name(record)['run_type'] for record in files_ZDC]
    zdc_HV_Value = [get_name(record)['HV_Value'] for record in files_ZDC]
elif partition == "HF":
    records = { file: get_name(file) for file in files_HF}
    zdc_PMT_ID = [get_name(record)['PMT_ID'] for record in files_HF]
    zdc_run_Type = [get_name(record)['run_type'] for record in files_HF]
    zdc_HV_Value = [get_name(record)['HV_Value'] for record in files_HF]


zdc_PMT_ID = list(set(zdc_PMT_ID))
zdc_run_Type = list(set(zdc_run_Type))
zdc_HV_Value = list(set(zdc_HV_Value))

print(zdc_PMT_ID, zdc_run_Type, zdc_HV_Value)

colors = [ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kMagenta]

for PMT_IDs in zdc_PMT_ID:
    print(f'For {PMT_IDs} PMT')

    # Canvas for LED and PED (HV=1100) runs
    canvas = ROOT.TCanvas(f"canvas_{PMT_IDs}", "Charge Distribution", 800, 600)
    ROOT.gStyle.SetOptStat(0)
    legend = ROOT.TLegend(0.5, 0.7, 0.9, 0.9)
    
    hist_dict_led = {}
    hist_ped_1100 = None
    color_index = 0
    hv_values = []
    gain_values = []

    for record in records:
        if records[record]['PMT_ID'] != PMT_IDs:
            continue
        
        run_type = records[record]['run_type']
        hv_value = records[record]['HV_Value']
        
        # Open ROOT file
        ftemp = ROOT.TFile.Open(record)
        
        # Access histogram 
        hist = ftemp.Get('AllSumCh/SumCharge_FIB_CH_0_FIBER_0')
        
        if run_type == "LED":
            if hv_value not in hist_dict_led:
                hist_dict_led[hv_value] = hist.Clone()
                hist_dict_led[hv_value].SetDirectory(0)
                hist_dict_led[hv_value].SetLineColor(colors[color_index % len(colors)])
                color_index += 1
            else:
                hist_dict_led[hv_value].Add(hist)
        
        elif run_type == "PED" and hv_value == 1100:
            if hist_ped_1100 is None:
                hist_ped_1100 = hist.Clone()
                hist_ped_1100.SetDirectory(0)
                hist_ped_1100.SetLineColor(ROOT.kBlack)
                legend.AddEntry(hist_ped_1100, f"PED HV = {hv_value}", "l")
            else:
                hist_ped_1100.Add(hist)
        
        ftemp.Close()
    
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
            
            # Calculate mean and width for the histogram
            mean = hist.GetMean()
            width = hist.GetRMS()
            gain_value = gain(mean, width)
            
            hv_values.append(hv_value)
            gain_values.append(gain_value)
            
            legend.AddEntry(hist, f"LED HV = {hv_value}, Gain = {gain_value:.2e}", "l")
    
    if hist_ped_1100:
        if first_hist:
            hist_ped_1100.Draw("HIST")
        else:
            hist_ped_1100.Draw("HIST SAME")
    
    # Set axis titles and ranges
    if hist_dict_led:
        first_hist = list(hist_dict_led.values())[0]
        first_hist.SetTitle(f"Charge Distribution for {PMT_IDs};Charge (fC);Entries")
        first_hist.GetXaxis().SetRangeUser(0, 1400)
        first_hist.GetYaxis().SetRangeUser(0, 2200)
    
    if hist_ped_1100:
        hist_ped_1100.SetTitle(f"Charge Distribution for {PMT_IDs};Charge (fC);Entries")
        hist_ped_1100.GetXaxis().SetRangeUser(0, 1400)
        hist_ped_1100.GetYaxis().SetRangeUser(0, 2200)
    
    legend.Draw()
    canvas.SaveAs(f"Results_Gain_Charge_{partition}/Charge_Distribution_{PMT_IDs}.pdf")
    canvas.Close()

    # Create a graph for HV vs Gain
    graph = ROOT.TGraph(len(hv_values), np.array(hv_values, dtype=float), np.array(gain_values, dtype=float))
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
    power_law = ROOT.TF1("power_law", "[0] * TMath::Power(x, 8*[1])", min(hv_values), max(hv_values))
    power_law.SetParameters(1, 1)  # Initial guess for the parameters

    # Perform the fit multiple times to improve results
    for i in range(25):
        graph.Fit(power_law, "R")

    power_law.Draw("SAME")

    # Add legend to show the fit result: k-value
    graph_legend = ROOT.TLegend(0.1, 0.7, 0.4, 0.8)
    graph_legend.AddEntry(power_law, f"k value = {power_law.GetParameter(1):.3e}", "l")
    graph_legend.Draw()

    canvas_graph.SaveAs(f"Results_Gain_Charge_{partition}/HV_vs_Gain_{PMT_IDs}.pdf")