# -*- coding: utf-8 -*-
'''
Author: U. Guney Tok

Description: Noise Measurement for the Signal Cables (lemo) on the Plus Side, underground.

Script usage: "python CSVtoRoot.py --section"
        e.g   python CSVtoRoot.py RPD
'''
import numpy as np
import ROOT
import sys
import os
import array
import matplotlib.pyplot as plt

# Load the necessary ROOT libraries
ROOT.gSystem.Load("libMathCore")
ROOT.gSystem.Load("libMathMore")

fileNames_Plus_Cable_Noise_RPD = ["ZDC_Plus_CableNoiseCSVs/1522269-rpd1.csv", "ZDC_Plus_CableNoiseCSVs/1522270-rpd2.csv", "ZDC_Plus_CableNoiseCSVs/1522271-rpd3.csv",
"ZDC_Plus_CableNoiseCSVs/1522272-rpd4.csv","ZDC_Plus_CableNoiseCSVs/1522273-rpd5.csv","ZDC_Plus_CableNoiseCSVs/1522274-rpd6.csv","ZDC_Plus_CableNoiseCSVs/1522275-rpd7.csv",
"ZDC_Plus_CableNoiseCSVs/1522276-rpd8.csv","ZDC_Plus_CableNoiseCSVs/1522277-rpd9.csv","ZDC_Plus_CableNoiseCSVs/1522278-rpd10.csv","ZDC_Plus_CableNoiseCSVs/1522279-rpd11.csv",
"ZDC_Plus_CableNoiseCSVs/1522280-rpd12.csv","ZDC_Plus_CableNoiseCSVs/1522281-rpd13.csv","ZDC_Plus_CableNoiseCSVs/1522282-rpd14.csv","ZDC_Plus_CableNoiseCSVs/1522283-rpd15.csv",
"ZDC_Plus_CableNoiseCSVs/1522284-rpd16.csv"]

fileNames_Minus_Cable_Noise_EM = ["ZDC_Plus_CableNoiseCSVs/1522343-em1.csv","ZDC_Plus_CableNoiseCSVs/1522344-em2.csv","ZDC_Plus_CableNoiseCSVs/1522345-em3.csv","ZDC_Plus_CableNoiseCSVs/1522346-em4.csv","ZDC_Plus_CableNoiseCSVs/1522347-em5.csv"]

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
else:
        print("Invalid section provided! Please use RPD, HAD, EM or Test.")
        sys.exit(1) 

for fileName in fileNames:
        count = count +1

        df = ROOT.RDF.MakeCsvDataFrame(fileName)

        rootfileName = "ZDC_Cable_%s/%s_rootFiles/%s_Channel_%s.root"%(section,section,section,count)
        treeName = "Channel_%s"%str(count)

        histo_csv = df.Histo1D(("Channel_%s"%count,"%s_Channel_%s; second (ms);Volt (mV)"%(section,count),200,0.0,0.06),"Volt")
        
        # Get number of bins in the histogram
        n_bins = histo_csv.GetNbinsX()

        data = [histo_csv.GetBinContent(i) for i in range(1, n_bins + 1)]

        # Perform FFT
        fft_result = np.fft.fft(data)
        magnitude = np.abs(fft_result)
        print(magnitude)

        # Draw magnitude plots per frequency bins
        plt.figure()
        plt.plot(np.arange(0, len(magnitude)), magnitude)
        plt.title("Magnitude Spectrum of FFT Result")
        plt.xlabel("Frequency Bin")
        plt.ylabel("Magnitude")
        plt.grid(True)
        plt.savefig("ZDC_Cable_%s/Histograms/%s_Channel_%s_FFT_Magnitude.pdf"%(section,section,str(count)))
        plt.close()   
        

        # Draw the Volt vs second 1D histograms
        df.Snapshot(treeName,rootfileName,"",snapshotOptions)
        f = ROOT.TFile.Open(rootfileName)
        c = ROOT.TCanvas()
        #c.SetLogx()
        c.SetLogy()
        histo_csv.Draw("pl")
        c.SaveAs("ZDC_Cable_%s/Histograms/%s_Channel_%s.pdf"%(section,section,str(count)))
