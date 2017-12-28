import matplotlib
matplotlib.use('Agg')
import pylab
import matplotlib.pyplot as plt
import csv
import sys

NUM_X_TICKS=20
NUM_Y_TICKS=7

FIG_WIDTH=25
FIG_HEIGHT=10

def msToReadable(ms):
  hours = ms // (1000 * 60 * 60)
  ms -= (hours * 1000 * 60 * 60)
  minutes = ms // (1000 * 60)
  ms -= (minutes * 1000 * 60)
  seconds = ms // 1000
  ms -= (seconds * 1000)
  return "%d:%02d:%02d.%03d" % (hours, minutes, seconds, ms)

def msToReadableShort(ms):
  hours = ms // (1000 * 60 * 60)
  ms -= (hours * 1000 * 60 * 60)
  minutes = ms // (1000 * 60)
  return "%d:%02d" % (hours, minutes)

def Plot(log_filename, plot_filename="plot.png"):
  with open(log_filename, 'rb') as f:
    reader = csv.reader(f, delimiter='\t')

    start_time = -1
    timestamps = []
    v1s = []
    v2s = []
    for row in reader:
      if start_time == -1:
        start_time = int(float(row[0]) * 1000)
      timestamps.append(int(float(row[0]) * 1000) - start_time)
      v1s.append(float(row[1][:-1]))
      v2s.append(float(row[2][:-1]))

  print "Read in %d entries." % len(timestamps)
  duration = timestamps[-1] - timestamps[0]
  print "Test duration: %s" % (msToReadable(duration))

  plt.figure(figsize=(FIG_WIDTH, FIG_HEIGHT))

  label_timestamps = [timestamps[i]
                      for i in range(0, len(timestamps), len(timestamps) / NUM_X_TICKS)]
  labels = [msToReadableShort(ts) for ts in label_timestamps]
  plt.xticks(label_timestamps, labels, rotation="vertical")
  plt.xlabel("Time Since Start")
  plt.subplots_adjust(bottom=0.2)

  plt.ylabel("Voltage")

  plt.title("Power Testing Results")

  plt.plot(timestamps, v1s, linestyle='-', label="LiPo Battery Voltage")
  plt.plot(timestamps, v2s, linestyle='-', label="Boost Converter Output")
  plt.legend()
  plt.grid()

  plt.savefig(plot_filename)

if __name__ == "__main__":
  if len(sys.argv) < 2:
    print 'ERROR: You must supply a log filename to open/plot'
    sys.exit(1)
  log_filename = sys.argv[1]
  print 'Opening "%s"...' % log_filename
  Plot(log_filename)
