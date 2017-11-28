import threading
import json
import logging
import time
import requests
from Queue import Queue
from requests.auth import HTTPBasicAuth

logger = logging.getLogger(__name__)


class StoppableThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.stop_event = threading.Event()

    def stop(self):
        if self.isAlive() == True:
            # set event to signal thread to terminate
            self.stop_event.set()
            # block calling thread until thread really has terminated
            self.join()


class webApi:
    def __init__(self,url='https://www.techgen.dk/AED/admin/get_drone_stop.php'):
        self.url = url
        self.username = ''
        self.password = ''

    def setAuthentication(self,_username,_password):
        self.username = _username
        self.password = _password

    def getAbortState(self):
        payload = {'drone_id': '1'}
        r = ''
        try:
            r = requests.post(url = self.url,auth=HTTPBasicAuth(self.username,self.password),data=payload)
        except:
            logger.error(' Error in polling abort state')
            return -1

        if r.text == '0': # Handle empty response
            logger.error(' Wrong request_id')
            return -2
        elif int(r.status_code) != 200: # Handle empty response
            logger.error(' Communication error')
            return -3

        try:
            jsonformat = json.loads(r.text) # convert to json
        except:
            logger.error('Unexpected error in parsing  as json')
            return -4

        print jsonformat
        print jsonformat[0]
        return int(jsonformat)


class WebInterface(StoppableThread):
    ABORT_STATE_NEUTRAL = 0
    ABORT_STATE_TERMINATE = 1
    ABORT_STATE_ABORT = 2
    ABORT_STATE_LAND = 3

    def __init__(self, gsm_command_queue, rate = 1):
        StoppableThread.__init__(self)
        self.rate = float(rate)
        self.command_queue = gsm_command_queue
        self.web_interface = webApi(url='https://www.techgen.dk/AED/admin/get_drone_stop.php')
        self.web_interface.setAuthentication('uasd','halogenlampe')


    def run(self):
        logger.info('web_interface started')
        while self.stop_event.is_set() is False:
            abort_state = self.web_interface.getAbortState()
            if abort_state == WebInterface.ABORT_STATE_NEUTRAL:
                logger.debug('Abort state: NEUTRAL')
            elif abort_state == WebInterface.ABORT_STATE_TERMINATE:
                msg = {'type': 'ACTION_TERMINATE'}
                self.command_queue.put(msg)
                logger.debug('Abort state: TERMINATE')
            elif abort_state == WebInterface.ABORT_STATE_ABORT:
                msg = {'type': 'ACTION_RETURN_TO_LAUNCH'}
                self.command_queue.put(msg)
                logger.debug('Abort state: ABORT')
            elif abort_state == WebInterface.ABORT_STATE_LAND:
                msg = {'type': 'ACTION_LAND_HERE'}
                self.command_queue.put(msg)
                logger.debug('Abort state: LAND')

            # Sleep to obtain desired rate
            time.sleep(1.0/self.rate)


def main():
    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger(__name__)
    logger.info('test')
    command_queue = Queue()
    webInterface = WebInterface(command_queue)
    webInterface.start()

    do_exit = False
    while do_exit == False:
        try:
            while not command_queue.empty():
                print 'Command queue:', command_queue.get()
            time.sleep(0.1)
        except KeyboardInterrupt:
            # Ctrl+C was hit - exit program
            do_exit = True

    webInterface.stop()

if __name__ == "__main__":
    main()