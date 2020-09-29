from manager.c_manager import Manager
from manager.config import Config
import time

if __name__ == '__main__':
    start = time.time()
    if True:#try:
        #Manager(Config.load_from_file("config.json")).train()
        Manager(Config.load_from_file("config.json")).run()
    #except Exception as e:
    #    print (e)
    #Manager(Config.load_from_file("config.json")).tune_offline()
    print("Total running time: {}".format(time.time() - start))