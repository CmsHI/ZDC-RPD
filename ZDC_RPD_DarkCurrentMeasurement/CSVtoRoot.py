# -*- coding: utf-8 -*-
'''
Author: U. Guney Tok

Description: Dark current of the PMTs from ZDC's RPD detectors.

Script usage: "python CSVtoRoot.py --region"
        e.g   python CSVtoRoot.py Plus
'''
import ROOT
import sys
import os

#fileNames_Minus=['230524_A1_Ch1.csv']
fileNames_Minus=["ZDC_Minus/ZDCM_waveform/230524_A1_Ch1.csv","ZDC_Minus/ZDCM_waveform/230524_A1_Ch2.csv","ZDC_Minus/ZDCM_waveform/230524_A1_Ch3.csv","ZDC_Minus/ZDCM_waveform/230524_A1_Ch4.csv","ZDC_Minus/ZDCM_waveform/230524_A2_Ch5.csv","ZDC_Minus/ZDCM_waveform/230524_A2_Ch6.csv","ZDC_Minus/ZDCM_waveform/230524_A2_Ch7.csv","ZDC_Minus/ZDCM_waveform/230524_A2_Ch8.csv","ZDC_Minus/ZDCM_waveform/230524_B3_Ch9.csv","ZDC_Minus/ZDCM_waveform/230524_B3_Ch10.csv","ZDC_Minus/ZDCM_waveform/230524_B3_Ch11.csv","ZDC_Minus/ZDCM_waveform/230524_B3_Ch12.csv","ZDC_Minus/ZDCM_waveform/230524_B4_Ch13.csv","ZDC_Minus/ZDCM_waveform/230524_B4_Ch14.csv","ZDC_Minus/ZDCM_waveform/230524_B4_Ch15.csv","ZDC_Minus/ZDCM_waveform/230524_B4_Ch16.csv"]

fileNames_Plus=["ZDC_Plus/ZDCP_waveform/230524_D1_Ch1.csv","ZDC_Plus/ZDCP_waveform/230524_D1_Ch2.csv","ZDC_Plus/ZDCP_waveform/230524_D1_Ch3.csv","ZDC_Plus/ZDCP_waveform/230524_D1_Ch4.csv","ZDC_Plus/ZDCP_waveform/230524_D2_Ch5.csv","ZDC_Plus/ZDCP_waveform/230524_D2_Ch6.csv","ZDC_Plus/ZDCP_waveform/230524_D2_Ch7.csv","ZDC_Plus/ZDCP_waveform/230524_D2_Ch8.csv","ZDC_Plus/ZDCP_waveform/230524_C3_Ch9.csv","ZDC_Plus/ZDCP_waveform/230524_C3_Ch10.csv","ZDC_Plus/ZDCP_waveform/230524_C3_Ch11.csv","ZDC_Plus/ZDCP_waveform/230524_C3_Ch12.csv","ZDC_Plus/ZDCP_waveform/230524_C4_Ch13.csv","ZDC_Plus/ZDCP_waveform/230524_C4_Ch14.csv","ZDC_Plus/ZDCP_waveform/230524_C4_Ch15.csv","ZDC_Plus/ZDCP_waveform/230524_C4_Ch16.csv"]

fileNames_Test = ["ZDC_Plus/ZDCP_waveform/230524_C4_Ch13.csv","ZDC_Plus/ZDCP_waveform/230524_C4_Ch14.csv"]

region = sys.argv[1]

snapshotOptions = ROOT.RDF.RSnapshotOptions()
snapshotOptions.fMode  = "RECREATE"
#snapshotOptions.fOverwriteIfExists = True

count = 0


if region == "Plus":
        fileNames = fileNames_Plus
elif region == "Minus":
        fileNames = fileNames_Minus
elif region == "Test":
        fileNames = fileNames_Test

for fileName in fileNames:
        count = count +1

        df = ROOT.RDF.MakeCsvDataFrame(fileName)

        rootfileName = "ZDC_%s/RPD_%s_rootFiles/RPD_%s_Channel_%s.root"%(region,region,region,count)
        treeName = "Channel_%s"%str(count)

        histo1D = df.Histo1D(("Channel_%s"%count,"RPD %s; Time (ns);Voltage (mV)"%region,150,-0.0002,0.001),"Voltage")

	histo1D.Fit("gaus","V","",0.0,0.0009)
	print("Channel_%s ------>"%str(count),histo1D.Integral())

	measure = histo1D.Integral()/50
	
	with open("Output_%s.txt"%region,"a") as text_file:
		text_file.write("Integral of Channel_%s == %s\n"%(str(count),measure))
        
	df.Snapshot(treeName,rootfileName,"",snapshotOptions)
        f = ROOT.TFile.Open(rootfileName)
        c = ROOT.TCanvas()
        #c.SetLogx()
        #c.SetLogy()
        histo1D.Draw("pl")
        c.SaveAs("ZDC_%s/Histograms/RPD%s_Channel_%s.png"%(region,region,str(count)))
