## This folder contains data and analysis code from the ZDC testbeam in 2023 August.

**All raw data files in xml format can be found in the** `raw_data` **folder**

The analysis code for each respective electron beam scan can be found in the folder `em{p or m for ZDC side}_{beam momentum in GeV}_e`.

# Run analysis code
1. To run analysis code you have to download the analysis folder you need and put the required xml data files in it (the list of required files is in the `files.txt` in the analysis folder). During analysis ROOT v6.28.04 was used.

2. After putting the data files in the folder you have to run the `convert_sec_to_root.C` macro, which will generate root files from the xmls.

3. Next step is to run the `make_pos_charge_root_update2.C` (for plus side) or `make_pos_charge_root_update2emm.C` (for minus side) macro with the first input parameter being the csv file (without file extension) in the analysis folder you are working in. (example: `make_pos_charge_root_update2emm.C("EMM_200Gev_e")`) This step generated a root file with the charge in each channel and table position for every event (which passed filters) in the input xml files.
4. The remaining macros are mainly for plotting and usually require as first input the root file's name without extension from the 3. step. (some macros need more input arguments, for which you can take a look in the macro)
5. To make the gain calibration you have to run the `gain_calib_strat2_gain{1..5}.C` macros with only one input parameter (this is the filename without extension from 3. step output). Channel 3 is used as reference for all cases, this is why there is no `gain_calib_strat2_gain3.C`, so the gains you get are only relative gains for one ZDC side at a time.
