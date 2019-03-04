from time import sleep
from selenium import webdriver
from os import popen
from serial import Serial
ThisUSB=''
# Loop until Spresense is found
while not ThisUSB:
    try:
        ThisUSB=popen("ls /dev/tty*USB*").readlines()[0].strip()
    except:
        pass
# Start Serial
ser = Serial(port = ThisUSB,baudrate = 9600)
# Location of driver
chrome_path = '/usr/lib/chromium-browser/chromedriver'
# Hide Chrome
chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--no-sandbox')
chrome_options.add_argument('--window-size=1420,1080')
chrome_options.add_argument('--headless')
chrome_options.add_argument('--disable-gpu')
# Launch browser
driver = webdriver.Chrome(executable_path=chrome_path,chrome_options=chrome_options)
# Go to DZ page
driver.get('http://houston.spacelandclock.com')
# Keep monitoring
while (1):
    for e in driver.find_element_by_id('clock').find_elements_by_xpath("*[contains(@id,'tm')]"):
        ThisText=e.text
        # If there are loads on the screen
        if (len(ThisText)>0):
            CallTime=int(ThisText.split()[2].split(':')[0].strip())
            # If the time is 5:XX, 10:XX , 15:XX, ...
            if (CallTime % 5 == 0 ):
                # Wait a minute to make the call only once
                sleep(60)
                CallCarrierType=ThisText.split()[0].split('-')[0].strip()
                CallCarrierNumber=ThisText.split()[1].strip()
                # Print for troubleshooting
                #print(CallCarrierType,CallCarrierNumber,CallTime)
                # Send to Spresense via Serial values separated by "|"
                ser.write(bytes(CallCarrierType + '|' + CallCarrierNumber + '|' + str(CallTime) + 'm','utf-8'))
