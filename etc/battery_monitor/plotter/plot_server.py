import time
import BaseHTTPServer
import plot

LOG_FILENAME="out.log"
PLOT_FILENAME="plot.png"


HOST_NAME = '0.0.0.0'
PORT_NUMBER = 8000

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_HEAD(s):
    s.send_response(200)
    s.send_header("Content-type", "text/html")
    s.end_headers()
  def do_GET(s):
    """Respond to a GET request."""
    plot.Plot(LOG_FILENAME, PLOT_FILENAME)
    s.send_response(200)
    s.send_header("Content-type", "image/png")
    s.end_headers()
    with open(PLOT_FILENAME, 'rb') as f:
      s.wfile.write(f.read())

if __name__ == '__main__':
    server_class = BaseHTTPServer.HTTPServer
    httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)
    print time.asctime(), "Server Starts - %s:%s" % (HOST_NAME, PORT_NUMBER)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print time.asctime(), "Server Stops - %s:%s" % (HOST_NAME, PORT_NUMBER)
