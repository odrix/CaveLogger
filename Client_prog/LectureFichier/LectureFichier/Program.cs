using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Ports;

namespace LectureFichier
{
    class Program
    {
        static void Main(string[] args)
        {
            bool isFinish = false;
            string filePath = @"C:\Users\Adrien\Documents\Arduino\CaveLogger\Data\result.csv";
            using (StreamWriter dumpFile = File.AppendText(filePath))
            {
                using (SerialPort serial = new SerialPort("COM3", 9600, Parity.None, 8, StopBits.One))
                {
                    string tmpData = "";
                    serial.Open();
                    serial.WriteLine("start");
                    while (!isFinish)
                    {
                        tmpData = serial.ReadLine();
                        isFinish = tmpData.Contains("finish");
                        if(!isFinish)
                            dumpFile.Write(tmpData);
                        
                    }
                    serial.WriteLine("end");
                    serial.Close();
                }
                dumpFile.Close();
            }
        }
    }
}
