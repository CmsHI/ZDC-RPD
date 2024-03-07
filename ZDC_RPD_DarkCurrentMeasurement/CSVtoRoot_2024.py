# -*- coding: utf-8 -*-
'''
Author: U. Guney Tok

Description: Dark current of the PMTs from ZDC's RPD detectors.

Script usage: "python CSVtoRoot_2024.py --section --region"
        e.g   python CSVtoRoot_2024.py RPD Plus 
'''
import ROOT
import sys
import os

fileNames_RPDPlus=["ZDC_March07/Data_RPD_07March/PLUS/RPD_CH1.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH2.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH3.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH4.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH5.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH6.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH7.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH8.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH9.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH10.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH11.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH12.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH13.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH14.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH15.csv","ZDC_March07/Data_RPD_07March/PLUS/RPD_CH16.csv"]

fileNames_RPDMinus=["ZDC_March07/Data_RPD_07March/MINUS/RPD_CH1.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH2.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH3.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH4.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH5.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH6.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH7.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH8.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH9.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH10.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH11.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH12.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH13.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH14.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH15.csv","ZDC_March07/Data_RPD_07March/MINUS/RPD_CH16.csv"]

fileNames_HADPlus=["ZDC_March07/Data_RP_HAD24/PLUS/HAD_CH1.csv","ZDC_March07/Data_RP_HAD24/PLUS/HAD_CH2.csv","ZDC_March07/Data_RP_HAD24/PLUS/HAD_CH3.csv","ZDC_March07/Data_RP_HAD24/PLUS/HAD_CH4.csv"]

fileNames_HADMinus=["ZDC_March07/Data_RP_HAD24/MINUS/HAD_CH1.csv","ZDC_March07/Data_RP_HAD24/MINUS/HAD_CH2.csv","ZDC_March07/Data_RP_HAD24/MINUS/HAD_CH3.csv","ZDC_March07/Data_RP_HAD24/MINUS/HAD_CH4.csv"]

fileNames_EMPlus = ["ZDC_March07/Data_RP_EM24/PLUS/EM_CH1.csv","ZDC_March07/Data_RP_EM24/PLUS/EM_CH2.csv","ZDC_March07/Data_RP_EM24/PLUS/EM_CH3.csv","ZDC_March07/Data_RP_EM24/PLUS/EM_CH4.csv","ZDC_March07/Data_RP_EM24/PLUS/EM_CH5.csv"]

fileNames_EMMinus = ["ZDC_March07/Data_RP_EM24/MINUS/EM_CH1.csv","ZDC_March07/Data_RP_EM24/MINUS/EM_CH2.csv","ZDC_March07/Data_RP_EM24/MINUS/EM_CH3.csv","ZDC_March07/Data_RP_EM24/MINUS/EM_CH4.csv","ZDC_March07/Data_RP_EM24/MINUS/EM_CH5.csv"]

section = sys.argv[1]
region = sys.argv[2]

snapshotOptions = ROOT.RDF.RSnapshotOptions()
snapshotOptions.fMode  = "RECREATE"
#snapshotOptions.fOverwriteIfExists = True

count = 0


if region == "Plus" and section == "RPD":
        fileNames = fileNames_RPDPlus
elif region == "Minus" and section == "RPD":
        fileNames = fileNames_RPDMinus
elif region == "Plus" and section == "HAD":
        fileNames = fileNames_HADPlus
elif region == "Minus" and section == "HAD":
        fileNames = fileNames_HADMinus
elif region == "Plus" and section == "EM":
        fileNames = fileNames_EMPlus
elif region == "Minus" and section == "EM":
        fileNames = fileNames_EMMinus              
else:
        print("Invalid section/region provided!")
        sys.exit(1)

for fileName in fileNames:
        count = count +1

        df = ROOT.RDF.MakeCsvDataFrame(fileName)

        rootfileName = "ZDC_2024_DarkCurrent_results/%s/%s_rootFiles/%s_%s_Channel_%s.root"%(section,region,section,region,count)
        treeName = "Channel_%s"%str(count)

        histo1D = df.Histo1D(("Channel_%s_%s"%(region,str(count)),"%s %s; Time (ns);Voltage (mV)"%(section,region),150,-0.0002,0.001),"Voltage")

	histo1D.Fit("gaus","V","",0.0,0.0009)
	print("Channel_%s_%s ------>"%(region,str(count)),histo1D.Integral())

	measure = histo1D.Integral()/50
	
	with open("Output_2024_%s.txt"%region,"a") as text_file:
		text_file.write("Integral of Channel_%s == %s\n"%(str(count),measure))
        
	df.Snapshot(treeName,rootfileName,"",snapshotOptions)
        f = ROOT.TFile.Open(rootfileName)
        c = ROOT.TCanvas()
        #c.SetLogx()
        #c.SetLogy()
        histo1D.Draw("pl")
        c.SaveAs("ZDC_2024_DarkCurrent_results/%s/Histogram_%s/%s_%s_Channel_%s.png"%(section,region,section,region,str(count)))
