# -*- coding: utf-8 -*-
'''
Author: U. Guney Tok

Description: Noise Measurement for the Signal Cables (lemo) on the Plus Side, underground.

Script usage: "python CSVtoRoot.py --section"
        e.g   python CSVtoRoot.py RPD
'''
from scipy import signal
import numpy as np
import ROOT
import sys
import os
import array
import matplotlib.pyplot as plt
from matplotlib import gridspec

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

fileNames_Spare = ["ZDC_Plus_CableNoiseCSVs/1522285-spare.csv"]

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
elif section == "Spare":
    fileNames = fileNames_Spare
else:
	print("Invalid section provided! Please use RPD, HAD, EM or Spare.")
	sys.exit(1) 

for fileName in fileNames:
    count = count +1

    df = ROOT.RDF.MakeCsvDataFrame(fileName)

    rootfileName = "ZDC_Cable_%s/%s_rootFiles/%s_Channel_%s.root"%(section,section,section,count)
    treeName = "Channel_%s"%str(count)

    histo1D = df.Histo1D(("Channel_%s"%count,"%s_Channel_%s; second (ms);Volt (mV)"%(section,count),100,0.035,0.055),"Volt")
	
    # Get number of bins in the histogram
    n_bins = histo1D.GetNbinsX()
    binMax = histo1D.GetMaximumBin()
    x_axis_Max = histo1D.GetBinCenter(binMax)
    print(int(x_axis_Max))
    data = [histo1D.GetBinContent(i) for i in range(1, n_bins + 1)]

    #Input square signal (from Bunker with Signal Generator)
    sample_rate = 10
    num_samples = 100
    wave = np.fromfunction(lambda i: (3*sample_rate < i) & (i < 7*sample_rate), (num_samples,)).astype(np.float)

    # Input square signal
    #input_signal = signal.square(np.linspace(0, 2 * np.pi, len(data)))	

    # Perform FFT
    fft_input = np.fft.fft(wave)
    fft_output = np.fft.fft(data)

    # Compute the magnitude spectrum of the FFT for both signals
    magnitude_input = np.abs(fft_input)
    magnitude_output = np.abs(fft_output)
    print('Magnitude Input',magnitude_output)
    print('Magnitude Output',magnitude_input)

    # Calculate the dispersion as the ratio of magnitudes
    dispersion = (magnitude_input / magnitude_output)

    #Normalize the data
    data = (data - np.min(data)) / (np.max(data) - np.min(data))

    # Plot the Square Signal, Output Signal and Dispersion
    fig = plt.figure(figsize=(10, 10))    
    gs = gridspec.GridSpec(2, 1, height_ratios=[2, 0.8])
    #Upper Plot
    ax1 = plt.subplot(gs[0])
    ax1.plot(wave, label='Square Signal', color='green')
    ax1.plot(data, label='Output Signal', color='red')
    ax1.set_ylabel('A.U')
    ax1.set_title("Dispersion by FFT of Input/Output Signal for %s Channel_%s"%(section, str(count)))
    ax1.legend()
    ax1.grid(True)
    ax1.set_ylim(0, 1.1)
    plt.setp(ax1.get_xticklabels(), visible=False)  
    
    #Lower plot
    ax2 = plt.subplot(gs[1])
    ax2.plot(np.arange(0, len(dispersion)), np.abs(dispersion), label='Dispersion', color='blue')
    ax2.set_xlabel("Frequency Bin")
    ax2.set_ylabel("input/output")
    ax2.legend()     
    #Adjust plot view
    plt.subplots_adjust(hspace=0.1)
    plt.xlabel("Frequency Bin")
    plt.grid(True)
    plt.legend()
    plt.savefig("ZDC_Cable_%s/Histograms/%s_Channel_%s_FFT_Magnitude.pdf"%(section,section,str(count)))
    plt.show()
    plt.close()   
	   
    df.Snapshot(treeName,rootfileName,"",snapshotOptions)
    f = ROOT.TFile.Open(rootfileName)
    c = ROOT.TCanvas()
    #c.SetLogx()
    #c.SetLogy()
    histo1D.Draw("pl")
    c.SaveAs("ZDC_Cable_%s/Histograms/%s_Channel_%s.pdf"%(section,section,str(count)))
