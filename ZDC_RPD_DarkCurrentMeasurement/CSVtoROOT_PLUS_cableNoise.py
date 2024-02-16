# -*- coding: utf-8 -*-
'''
Author: U. Guney Tok

Description: Noise Measurement for the Signal Cables (lemo) on the Plus Side, underground.

Script usage: "python CSVtoRoot.py --section"
        e.g   python CSVtoRoot.py RPD
'''
import ROOT
import sys
import os

fileNames_Plus_Cable_Noise_RPD = ["ZDC_Plus_CableNoiseCSVs/1522269-rpd1.csv", "ZDC_Plus_CableNoiseCSVs/1522270-rpd2.csv", "ZDC_Plus_CableNoiseCSVs/1522271-rpd3.csv",
"ZDC_Plus_CableNoiseCSVs/1522272-rpd4.csv","ZDC_Plus_CableNoiseCSVs/1522273-rpd5.csv","ZDC_Plus_CableNoiseCSVs/1522274-rpd6.csv","ZDC_Plus_CableNoiseCSVs/1522275-rpd7.csv",
"ZDC_Plus_CableNoiseCSVs/1522276-rpd8.csv","ZDC_Plus_CableNoiseCSVs/1522277-rpd9.csv","ZDC_Plus_CableNoiseCSVs/1522278-rpd10.csv","ZDC_Plus_CableNoiseCSVs/1522279-rpd11.csv",
"ZDC_Plus_CableNoiseCSVs/1522280-rpd12.csv","ZDC_Plus_CableNoiseCSVs/1522281-rpd13.csv","ZDC_Plus_CableNoiseCSVs/1522282-rpd14.csv","ZDC_Plus_CableNoiseCSVs/1522283-rpd15.csv",
"ZDC_Plus_CableNoiseCSVs/1522284-rpd16.csv"]

fileNames_Minus_Cable_Noise_EM = ["ZDC_Plus_CableNoiseCSVs/1522343-em1.csv","ZDC_Plus_CableNoiseCSVs/1522344-em2.csv","ZDC_Plus_CableNoiseCSVs/1522345-em3.csv","ZDC_Plus_CableNoiseCSVs/1522346-em4.csv"]

fileNames_Minus_Cable_Noise_HAD = ["ZDC_Plus_CableNoiseCSVs/1522348-had1.csv","ZDC_Plus_CableNoiseCSVs/1522266-had2.csv","ZDC_Plus_CableNoiseCSVs/1522267-had3.csv","ZDC_Plus_CableNoiseCSVs/1522268-had4.csv"]

fileNames_Test = ["ZDC_Plus_CableNoiseCSVs/1522285-spare.csv"]

section = sys.argv[1]

snapshotOptions = ROOT.RDF.RSnapshotOptions()
snapshotOptions.fMode  = "RECREATE"
#snapshotOptions.fOverwriteIfExists = True

count = 0


if section == "RPD":
        fileNames = fileNames_Plus_Cable_Noise_RPD
elif section == "HAD":
        fileNames = fileNames_Minus_Cable_Noise_HAD
elif section == "EM":
        fileNames = fileNames_Minus_Cable_Noise_EM
elif section == "Test":
        fileNames = fileNames_Test

for fileName in fileNames:
        count = count +1

        df = ROOT.RDF.MakeCsvDataFrame(fileName)

        rootfileName = "ZDC_Cable_%s/%s_rootFiles/%s_Channel_%s.root"%(section,section,section,count)
        treeName = "Channel_%s"%str(count)

        histo1D = df.Histo1D(("Channel_%s"%count,"%s_Channel_%s; second (ms);Volt (mV)"%(section,count),150,-0.0002,0.001),"Volt")

	#histo1D.Fit("gaus","V","",0.0,0.0009)
	print("Channel_%s ------>"%str(count),histo1D.Integral())

	#measure = histo1D.Integral()/50
	
	#with open("Output_%s.txt"%section,"a") as text_file:
	#	text_file.write("Integral of Channel_%s == %s\n"%(str(count),measure))
        
	df.Snapshot(treeName,rootfileName,"",snapshotOptions)
        f = ROOT.TFile.Open(rootfileName)
        c = ROOT.TCanvas()
        #c.SetLogx()
        c.SetLogy()
        histo1D.Draw("pl")
        c.SaveAs("ZDC_Cable_%s/Histograms/%s_Channel_%s.pdf"%(section,section,str(count)))
