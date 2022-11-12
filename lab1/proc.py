import pandas as pd 

old_dataframe=pd.read_csv('proc.csv', sep=',', header=0)
new_dataframe=pd.read_csv('in.csv', sep=',', header=0)
#print(new_dataframe)
#print(old_dataframe)
if ('Rl' in new_dataframe["STAT"].values):
        new_dataframe["STAT"] = new_dataframe["STAT"].replace(['Rl'],1)

if ('Sl' in new_dataframe["STAT"].values):
        new_dataframe["STAT"] = new_dataframe["STAT"].replace(['Sl'],2)

if (len(old_dataframe.columns) == 1):
        new_dataframe.columns =  ['0']
else: 
        new_dataframe.columns = [str(int(old_dataframe.columns[-1]) + 1)]
result = pd.concat([old_dataframe, new_dataframe], axis=1)
result.to_csv('proc.csv', index=False)
#print(result)

