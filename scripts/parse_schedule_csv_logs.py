#### This is an analysis script for creating plots of entering_times against timestamps of vehicles using scheduling_service logs
#### The plots are created for EV and DV states and require a scheduling_service log csv to be passed as an argument

### Additional Note
#### A single plot consists of entering times for a run, which is marked by linebreaks in the csv. Current implementation adds line breaks when there is no vehicle in
#### the intersection and only has entries when the vehicle has carma engaged.
#### This was a useful demarcation for separting runs but the script might need to be modified if implementation changes.

import csv
from time import time
import matplotlib
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as md 
import sys
import pandas as pd
import math
import matplotlib.patches as mpatches
import numpy as np

class Logparser():
    
    log_file_name_ = ""
    num_of_runs_ = 0
    ev_runs_to_count_ = 10

    def __init__(self, file_name):
        self.log_file_name_ = file_name
        self.process_runs()

    def convert_to_datetime(self, x):
        return datetime.datetime.fromtimestamp(int(x)/1000)


    def plot_run(self, df):
        # Method to handle processing of single run stored in pandas dataframe df

        # Separate out values based on states
        df_ev = df[df["state"] == "EV"]
        df_dv  = df[df["state"] == "DV"]


        # Convert timestamps and entering timestamps to datetime values
        pd.options.mode.chained_assignment = None  # default='warn'
        df_ev['timestamps'] = df_ev['timestamps'].apply(self.convert_to_datetime)
        df_ev['et'] = df_ev['et'].apply(self.convert_to_datetime)

        df_dv['timestamps'] = df_dv['timestamps'].apply(self.convert_to_datetime)
        df_dv['et'] = df_dv['et'].apply(self.convert_to_datetime)

        
        import matplotlib.dates as mdates
        myFmt_timestamp = mdates.DateFormatter('%H:%M:%S') # here you can format your datetick labels as desired
        myFmt_ets = mdates.DateFormatter('%H:%M:%S%d')

        fig, ax = plt.subplots()
        ax.plot(df_ev['timestamps'], df_ev['et'], color = 'red')

        ax.plot(df_dv['timestamps'], df_dv['et'], color = 'blue')
        
        plt.tight_layout()
        plt.gca().xaxis.set_major_formatter(myFmt_timestamp)
        plt.gca().yaxis.set_major_formatter(myFmt_ets)
        
        ax.set_xlabel('Timestamps (hours:mins:seconds)')
        ax.set_ylabel('Entering Times (hours:mins:seconds)')
        ev_red_patch = mpatches.Patch(color='red', label='EV')
        dv_blue_patch = mpatches.Patch(color='blue', label='DV')
        plt.legend(handles=[ev_red_patch, dv_blue_patch], title="State")

        plt.show()        
        




    def process_runs(self):


        #####################################################################################
        df = pd.read_csv(self.log_file_name_, names=('timestamps', 'vehicle_id', 'entry_lane', 'link_id', 
        'eet', 'et', 'dt', 'state'), skip_blank_lines=False)
        

        # Separate runs based on line breaks
        df_list = np.split(df, df[df.isnull().all(1)].index) 
        
        run = 0
        for df in df_list:
            # Drop all NA values
            df = df.dropna()
            if not df.empty:
                run += 1
                self.plot_run(df)
                # print(df)
                
                

        print("Number of runs: ", run)

if __name__ == '__main__' :
    if not len(sys.argv) > 1:
        print("Error: This file needs to be run with the name of the csv log file to process")
        exit()

    file_name = sys.argv[1]
    worker = Logparser(file_name) 