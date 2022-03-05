using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;

namespace SwifTurk_GCS_Comm
{
	public partial class Form1 : Form
	{

		public enum Command_Types : byte
		{
			MANUAL_RELEASE = 0,
			TEST_MOTOR	   = 1
		};
		public enum LastSendedPckgType : byte
        {
			VIDEO_FRAME	= 0,
			COMMAND		= 1,
			REQUEST		= 2
        };


		public enum Package_Types : byte
		{
			TELEM_1HZ				= 0,
			TELEM_FLIGHT_HEALTH		= 1,
			GCS_TELEM_REQUEST		= 2,
			GCS_TELEM_MISSION		= 3,
			GCS_TELEM_COMMAND		= 4,
			GCS_TELEM_RESPONSE		= 5,
			CONTAINER_TELEMETRY		= 6,
			VIDEO_PACKAGE_REQUEST	= 7,
			UNKNOWN
		};
		
		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public unsafe struct Telemetry_1HZ_t
		{
			/* Main Telemetry Strcuture , Takes Sensore Data and
			 * Collect Container Telemetry Data.
			 */


			public uint START_OF_FRAME; // <
			public byte PACKAGE_TYPE; // this is 1Hz telemetry Structure @param => enum PACKAGE_TYPES
			public byte PACKAGE_SIZE;
			public ushort TEAM_ID;
			public ushort PACKET_NUMBER;  // will be increased every Hz



			public float Temperature;
			public float Pressure;
			public float Altitude;  // relative to Y-Axis
			public float DescentSpeed;  // relative to Y-Axis

			public float GPS_Latitude;
			public float GPS_Longtitude;
			public float GPS_Altitude;

			public float Container_Altitude;
			public float Container_Pressure;
			public float Container_GPS_Latitude;
			public float Container_GPS_Longtitude;
			public float Container_GPS_Altitude;
			public float AltitudeDifference;

			public float pitch;
			public float roll;
			public float yaw;

			public byte FLIGHT_STATUS;                          // for GCS Define => enum FLIGHT_STATUS_t : byte
			public byte VIDEO_TRANSMISSION_STATUS;  //

			public uint END_OF_FRAME; // >
		};

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public unsafe struct GCS_Comamnd_t
		{
			public uint START_OF_FRAME; // <

			public byte PACKAGE_TYPE;

			public byte COMMAND;

			public uint END_OF_FRAME; // >
		};

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public unsafe struct GCS_Video_t
		{
			public uint START_OF_FRAME; // <

			public byte PACKAGE_TYPE;

			public ushort videoID;

			public unsafe fixed byte videoByte[255]; // Bunu 255 e çek. ve yukarıya uint8_t bir videoSize koy. yani ornegin 255 byte lık bir array'de (Sonlara doğru) belki son paket

			public byte isEnd;
			public uint END_OF_FRAME; // >
		};


		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public unsafe struct PayloadVideoHandler_t
		{
			public uint START_OF_FRAME; // <

			public byte PACKAGE_TYPE;

			public ushort videoID;

			public uint END_OF_FRAME; // >
		};
		[StructLayout(LayoutKind.Sequential, Pack = 1)]

		public unsafe struct GCS_Tel_Response_t
		{
			public uint START_OF_FRAME; // <

			public byte PACKAGE_TYPE;

			public byte RESPONSE_STATUS;

			public uint END_OF_FRAME; // >
		}

		public byte[] videoBuffer = new byte[3500];

		public static GCS_Video_t				gcsVideoPacket;
		public static GCS_Comamnd_t			    gcsCommandPacket;
		public static Telemetry_1HZ_t			Telemetry_1HZ_Packet;
		public static PayloadVideoHandler_t	    PayloadVideoHandlerPacket;
		public static GCS_Tel_Response_t		PayloadResponsePacket;

		

		public static bool isSerialBusy		= false;
		public static bool readedAVI		= false;
		public static bool readignStarted	= false;
		public static bool isRdngCompleted	= false;
		public static bool readingAllowed	= true;
		public static bool timerFlagActive	= false;
		public static bool commandActive	= false;


		public static volatile bool isSerialSenderBusy	 = false;
		public static volatile bool isVideoSendingEnable = false;
		public static volatile bool waitforConformation	 = false;


		public static byte[] packetArr = new byte[150];
		public static int packetArrCt = 0;
		public static volatile int testVideoCt = 0;
		public static byte[] packetArr_TEST = new byte[150];
		public static int commandTimerFlag = 0;


		public static Stopwatch timePassedWatch = new Stopwatch();
		public static Stopwatch timePassedWatchVideo = new Stopwatch();

		public static string[] SATELLEITE_STATE_STR    = { "Bekleme", "Yukselme", "Model Uydu Inis", "Ayrilma", "Gorev Yuku Inis", "Irtifa Sabit", "Kurtarma" };
		public static string[] LABEL_STRINGS = { "Team ID : ", "Packet Number : ", "Temperature : " ,"Pressure : " ,"Altitude : ","Descent Speed : ","GPS Latitude : ","GPS Longtitude : ","GPS Altitude : ","Container Altitude : ","Container Pressure : ","Container GPS Latitude : ","Container GPS Longtitude : ","Container GPS Altitude : ","Altitude Difference : ","Pitch : ","Roll : ","Yaw : ","Flight Status : ","Video Transmission Status : "};
		public static int sendedVideoSize = 0;
		public static LastSendedPckgType LastSended = LastSendedPckgType.COMMAND;
		
		
		

		public Form1()
        {
			Label.CheckForIllegalCrossThreadCalls = false;

			InitializeComponent();
			initStructures();
			string[] ports = SerialPort.GetPortNames();

			Console.WriteLine("The following serial ports were found:");

			// Display each port name to the console.
			foreach (string port in ports)
			{
				Console.WriteLine(port);
			}
		}


		/* Gerekli paketlerin initlerini yapıyoruz.
		 *  Örneğin :
		 *			Start header : 0x5A5A5A5A
		 *			EOF   header : 0x5A5A5A5A
		 *			gibi..
		 */
		public unsafe void initStructures()
        {
			// Note :  gcsVideoPacket.videoByte fixed olduğu için, fonksiyonlarda değerini değiştirmek için
				// o tanımlı fonksiyon unsafe olmalı.
			// GcsVideoPacket
			gcsVideoPacket.START_OF_FRAME	= 0x5A5A5A5A;
			gcsVideoPacket.END_OF_FRAME		= 0x5A5A5A5A;
			gcsVideoPacket.isEnd			= 0;
			gcsVideoPacket.PACKAGE_TYPE		= (byte)Package_Types.GCS_TELEM_MISSION;
			gcsVideoPacket.videoID			= 1;
			gcsVideoPacket.videoByte[0]		= 0;
			gcsVideoPacket.videoByte[20]	= 22;


			gcsCommandPacket.START_OF_FRAME = 0x5A5A5A5A;
			gcsCommandPacket.END_OF_FRAME	= 0x5A5A5A5A;
			gcsCommandPacket.PACKAGE_TYPE	= (byte)Package_Types.GCS_TELEM_COMMAND;
			gcsCommandPacket.COMMAND		= (byte)Command_Types.MANUAL_RELEASE;
			//testPrint();

			
			//int size =  Marshal.SizeOf(gcsVideoPacket);
			//Console.WriteLine(size);
		}

		public unsafe void testPrint()
        {
			for (byte i = 0; i < 200; i++)
			{
				Console.WriteLine($"index = {i} , ValueofArr : {gcsVideoPacket.videoByte[i]}");
			}
		}

		public void clearBUFFER()
        {
			for (byte i = 0; i < 150; i++)
            {
				packetArr[i] = 0;
            }
        }

		public void printLabels( object sender, EventArgs  e)
        {
			int arrct = 0;
			team_id_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.TEAM_ID.ToString()}";
			packet_number_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.PACKET_NUMBER.ToString()}";
			temperature_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Temperature.ToString()}";
			pressure_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Pressure.ToString()}";
			altitude_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Altitude.ToString()}";
			descent_speed_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.DescentSpeed.ToString()}";
			gps_latitude_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.GPS_Latitude.ToString()}";
			gps_longitude_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.GPS_Longtitude.ToString()}";
			gps_alt_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.GPS_Altitude.ToString()}";

			conainer_alt_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Container_Altitude.ToString()}";
			container_pres_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Container_Pressure.ToString()}";

			container_gps_lat_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Container_GPS_Latitude.ToString()}";
			container_gps_long_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Container_GPS_Longtitude.ToString()}";
			container_gps_alt_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.Container_GPS_Altitude.ToString()}";
			alt_difference_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.AltitudeDifference.ToString()}";

			pitch_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.pitch.ToString()}";
			roll_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.roll.ToString()}";
			yaw_label.Text = $"{LABEL_STRINGS[arrct++]}{Telemetry_1HZ_Packet.yaw.ToString()}";
			flight_status_label.Text = $"{LABEL_STRINGS[arrct++]}{SATELLEITE_STATE_STR[Telemetry_1HZ_Packet.FLIGHT_STATUS]}";
			video_transmission_label.Text = $"{LABEL_STRINGS[arrct++]}{(Telemetry_1HZ_Packet.VIDEO_TRANSMISSION_STATUS == 1  ? "EVET" : "HAYIR")}";
			
        }


		/* Telemetri paketinin çözülmesine yardımcı olan fonksiyondur.
		 *  Telemetri paketinin 5.byte'ı bizlere hangi paket tipini olduğunu söylemektedir.
		 *  Bu 5.byte'a göre programımız gerekli paket kopyalama işlemlerini gerçekleştirir.
		 *  
		 *	Eğer model uydu tarafından video paketi talep edildi ise, yer istasyonumuz bu paket talebine
		 *	yeni bir video ID'ye sahip paket ya da istenilen video ID'ye sahip paket oluşturarak model uydumuza göndermek üzere
		 *	hazırlıklar yapar.
		 */
		public void solveTelemetryPackage(object sender, EventArgs e) //object sender , EventArgs e

		{
			//Console.WriteLine($"Data package is solving......{packetArrCt}");
			IntPtr emptyPointer = Marshal.AllocHGlobal(packetArrCt);
			Marshal.Copy(packetArr, 0, emptyPointer, packetArrCt);
			if ( packetArr[4] == (byte)Package_Types.TELEM_1HZ )
            {
				//Console.WriteLine("Telemetry Package TRYING TO UNPACKAGE.");
				Telemetry_1HZ_Packet = (Telemetry_1HZ_t)Marshal.PtrToStructure(emptyPointer, Telemetry_1HZ_Packet.GetType());
				Console.WriteLine($"Telemetry Package has been unpackaged. Son Telemetriden sonra geçen zaman (Elapsed): {timePassedWatch.ElapsedMilliseconds}");
				timePassedWatch.Restart();
				Console.WriteLine($"Package No : {Telemetry_1HZ_Packet.PACKET_NUMBER} , State : {SATELLEITE_STATE_STR[Telemetry_1HZ_Packet.FLIGHT_STATUS]}");
				this.Invoke(new EventHandler(printLabels));

				// Grafiklere bas.
			}
			else if ( packetArr[4] == (byte)Package_Types.VIDEO_PACKAGE_REQUEST )
            {
                videoTimer.Stop();
                Console.WriteLine("Payload Video Package TRYING TO UNPACKAGE.");
				PayloadVideoHandlerPacket = (PayloadVideoHandler_t)Marshal.PtrToStructure(emptyPointer, PayloadVideoHandlerPacket.GetType());
				Console.WriteLine($"PayloadVideoHandler  Package has been unpackaged. Req ID : {PayloadVideoHandlerPacket.videoID}");
				// Payload tarafından talep edilen ID bizim son gönderdiğimiz videoID+1 ise yeni videoByte gönder.
				if (PayloadVideoHandlerPacket.videoID == gcsVideoPacket.videoID+1)
                {
					Console.WriteLine("Gonderilen Video Alinmis Yenisi Gonderiliyor..");
					gcsVideoPacket.videoID += 1;
					testVideoCt += 1;
					sendedVideoSize += 255;
					sended_video_size_label.Text = $"Sended Video Size : {sendedVideoSize}";
				}
				else
                {
					Console.WriteLine($"ERRRRRRRRRRRRRRRRRRRRRRRR Req ID : {PayloadVideoHandlerPacket.videoID} our ID : {gcsVideoPacket.videoID}");
					gcsVideoPacket.videoID = PayloadVideoHandlerPacket.videoID;
                }
				if (!commandActive)
                {
					waitforConformation = false;
				}
                
            }
			else if (packetArr[4] == (byte)Package_Types.GCS_TELEM_RESPONSE ) 
            {
				/* Bir şeylerin ters gittiğini model uydumuz bizlere söylyüyor ise.
				 *  Yer istasyonu en son gönderilen paketin ne olduğuna dair araştırma yapar, ve tekrar gönderir.
				 */
				Marshal.FreeHGlobal(emptyPointer);
				serialFlagsPermission();

				Console.WriteLine("!------------------------------------------------RESPONSE ERROR!------------------------------------------------!");
				if ( ( LastSendedPckgType.COMMAND == LastSended ) &&  ( false == isVideoSendingEnable ) )
                {
					Console.WriteLine("------------------------------------------------COMMAND ERROR!");
					serialSenderFunction( Package_Types.GCS_TELEM_COMMAND );
					
                }
				else
                {
					Console.WriteLine("------------------------------------------------COMMAND&VIDEO ERROR!");
					serialSenderFunction(Package_Types.GCS_TELEM_COMMAND);
					waitforConformation = false;
				}
				if ( LastSendedPckgType.VIDEO_FRAME == LastSended && ( false == isVideoSendingEnable ) )
                {
					Console.WriteLine("------------------------------------------------VIDEO ERROR!");
					waitforConformation = false;
				}
				return;
            }

			
			Marshal.FreeHGlobal(emptyPointer);

			serialFlagsPermission();
		}


		/* serialFlagsPermission fonksiyonu, serialBuffer'ı temizleyip, yeni gelecek olan paketler için hazırlık yapar.
		 * @retval void
		 */
		public void serialFlagsPermission()
        {
			clearBUFFER();
			packetArrCt = 0;
			isRdngCompleted = false;
			isSerialBusy = false;
			readingAllowed = true;
		}


		/* threadFunction bu fonksiyon, serial üzerdinden paket taraması yapar.
		 *  Aynı zamanda, video gönderim fonksiyonu aktive durumda ise, video paketi gönderme işlemini sağlar.
		 */
		public unsafe void threadFunction ()
		{
			bool findAgain = false;
			for (; ; )
            {
				if (serialPortComm.BytesToRead > 0 && readingAllowed)
				{
					byte readedByte = (byte)serialPortComm.ReadByte();
                    packetArr[packetArrCt] = readedByte;
                    //Console.WriteLine($"Readed byte : {readedByte}");
                    if (packetArrCt >= 3) // Paket başlangıcı 0x5a5a5a5a olduğundan gelen pakette header'ı arar.
                    {
						uint FrameChecker = (uint)(packetArr[packetArrCt-3] << 24 | packetArr[packetArrCt-2] << 16 | packetArr[packetArrCt-1] << 8 | packetArr[packetArrCt]);
						if (FrameChecker == 0x5A5A5A5A)
                        {
							
							if (findAgain)
                            {
								Console.WriteLine("Paket Header : 5a5a5a5a Bulundu !");
								readingAllowed = false;
                                findAgain = false;
                                this.Invoke(new EventHandler(solveTelemetryPackage));

                                //packetArrCt = 0;// temporaraly
								continue;
                            }
							findAgain = true;
						}
						
					}
					packetArrCt += 1;

				}
				if ( ( isVideoSendingEnable && !waitforConformation ) && !isSerialSenderBusy ) // Video gönderim aktif ise.
                {
					if ( !timerFlagActive )
                    {
						// eğer MCU'dan gerekli sürede cevap gelmediyse timer aktive olup paketi tekrar gönderecektir.
						this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Enabled = true; });
						this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Stop(); });
						this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Start(); });
						timerFlagActive = true;
					}
                    else
                    {
						// eğer MCU'dan gerekli sürede cevap gelmediyse timer aktive olup paketi tekrar gönderecektir.
						this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Stop(); });
                        this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Start(); });
                    }
					waitforConformation = true; // videoPaket gönderme modundan cevap bekleme moduna geç.
					bufferSwap(); // video buffer'ını değiştir.
					serialSenderFunction(Package_Types.GCS_TELEM_MISSION); // Video paketini gönder.
					Console.WriteLine($"sender:{isSerialSenderBusy} , vidEn {isVideoSendingEnable}, conf:{waitforConformation} , isEnd : {gcsVideoPacket.isEnd} , CT : {testVideoCt}");
				}
			}
			
        }

		// Seri haberleşmeyi başlatıyoruz.
        private void serialStartButton_Click(object sender, EventArgs e)
        {
            serialPortComm.Open();
            Thread pollingSerialFunc = new Thread(new ThreadStart(threadFunction));
			pollingSerialFunc.Start();
			serialStartButton.BackColor = Color.Green;
		}



		/* serialSenderFunction fonksiyonu parametre olarak verilen paket tipine göre,
		 * model uyduya dataları structure olarak paket halinder gönderir.
		 * 
		 */
		public void serialSenderFunction( Package_Types whichPackage )
        {
			isSerialSenderBusy = true;

			int structureSize = 0;
			byte[] byteArr;
			IntPtr ptr;

			switch ( whichPackage )
            {
				case Package_Types.GCS_TELEM_COMMAND:
					structureSize = Marshal.SizeOf(gcsCommandPacket);
					LastSended = LastSendedPckgType.COMMAND;
					break;
				case Package_Types.GCS_TELEM_MISSION:
					structureSize = Marshal.SizeOf(gcsVideoPacket);
					LastSended = LastSendedPckgType.VIDEO_FRAME;
					break;
				default:
					break;
            }

			byteArr = new byte[structureSize];
			ptr = Marshal.AllocHGlobal(structureSize);
			
			if (whichPackage == Package_Types.GCS_TELEM_COMMAND)
            {
                Marshal.StructureToPtr(gcsCommandPacket, ptr, true);
            }
			else
            {
				Marshal.StructureToPtr(gcsVideoPacket, ptr, true);
			}
            Marshal.Copy(ptr, byteArr, 0, structureSize);
            Marshal.FreeHGlobal(ptr);

			serialPortComm.Write(byteArr, 0, byteArr.Length);

			isSerialSenderBusy = false;
		}



		/* checkVideoActiveforCommandSending fonksiyonu eğer video gönderimi aktif ise,
		 * video göndermini kısa süreli durdurmaya yarar.
		 * 
		 * Amaç : MCU RX IDLE tabanlı çalıştığı için paketi beklemeden gönderirsek, video paketi ile karışma ihtimali yüksektir.
		 * 
		 */
		public void checkVideoActiveforCommandSending()
		{
			if (isVideoSendingEnable) 
			{
				waitforConformation = true;
				commandActive = true;
			}
		}

		/* activateCommandTimer fonksiyonu gerekli flag'leri değiştirerek commandTimer'ı aktife hale getirir
		 * 
		 * 
		 * Amaç : MCU RX IDLE tabanlı çalıştığı için paketi beklemeden gönderirsek, video paketi ile karışma ihtimali yüksektir.
		 * 
		 */
		public void activateCommandTimer()
		{
			commandTimerFlag = 1;
			this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { commandTimer.Enabled = true; });
			this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { commandTimer.Stop(); });
			this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { commandTimer.Start(); });
		}


		/* Kullanıcı tarafından manuel ayrışma talebi alındı ise bu event  fonksiyonu tetiklenir.
		 */
		private void releaseCommandButton_Click(object sender, EventArgs e)
        {
			checkVideoActiveforCommandSending();
			gcsCommandPacket.COMMAND = (byte)Command_Types.MANUAL_RELEASE;
			activateCommandTimer();
        }




		/* readAVI fonksiyonu altında gerekli video dosyasından binary reading yapılmalıdır. 
		 * 
		 */
		public void readAVI()
        {
			// okunan byte'ları videoBuffer'a kopyle.
			return;
        }


		/* bufferSwap fonksiyonu gönderilen video paketinin boyutu 300kb'a ulaştıysa, gönderimi durdurur.
		 *	Model uyduya gönderilecek paketin son paket olduğunu söyleyen bir ibare de bırakır.
		 */
		public unsafe int bufferSwap()
        {
			// Burda videoBuffer'dan al, gcsVideoPacket.videoByte'a aktar.
			
			for (byte i = 0; i < 255; i++)
            {
				gcsVideoPacket.videoByte[i] = i;
            }

			if (testVideoCt == 1176) // Yaklasik 300kb. 
            {
				isVideoSendingEnable = false;
				timePassedWatchVideo.Stop();
				long elapsedTime = timePassedWatchVideo.ElapsedMilliseconds;
				String elapsedString = elapsedTime.ToString();
				Console.WriteLine($"Elapsed Video Sending Process : {elapsedTime} milisaniye.");
				videoTimePassLabel.Text = $"Elapsed Video Time : {elapsedString} ms";
				gcsVideoPacket.isEnd = 1;
				this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Enabled = false; });
				this.BeginInvoke((System.Windows.Forms.MethodInvoker)delegate () { videoTimer.Stop(); });
				sendVideoButton.BackColor = Color.Green;
				return 0;
            }
			return 1;
        }


		/* Kullanıcı tarafından video paketi gönderimini başlatan event fonksiyonudur.
		 */
		private void sendVideoButton_Click(object sender, EventArgs e)
        {
			if ( !readedAVI )
            {
				readAVI();

				isVideoSendingEnable = true;
				waitforConformation = false;
				readedAVI = true;
                timePassedWatchVideo.Start();
				
			}
			sendVideoButton.BackColor = Color.Orange;
			
		}

		/* videoTimer_Tick event fonksiyonu, model uydumuzdan gönderilen vdeo paketine cevap gelmediyse tetiklenir ve
		 * Video paketi gönderme işlemi cevap bekleme pozisyonunda iken tekrar gönderme işlevine geri döner.
		 */
		private void videoTimer_Tick(object sender, EventArgs e)
        {
			Console.WriteLine("Ticking**************************");
            waitforConformation = false;
			videoTimer.Enabled = true;
			videoTimer.Stop();
			videoTimer.Start();
        }


		/* commandTimer_Tick event fonksiyonu, Komut paketi gönderilmek istediği zaman aktive hale gelen bir timer tick event fonksiyonudur.
		 *  İlk tick'te komu paketi gönderilir,  beklenilen diğer tickte eğer video paketi aktif halde ise durdurulmasına sebep olan flag'ler eski haline döndürülür.
		 */
		private void commandTimer_Tick(object sender, EventArgs e)
        {
			if (commandTimerFlag == 1)
            {
				serialSenderFunction(Package_Types.GCS_TELEM_COMMAND);
				commandTimerFlag = 2;
				Console.WriteLine("KOMUT GONDERILDI BIDAHAKI 500MS DE VIDEO GONDERILMEYE BASLANACAK.");
				commandTimer.Stop();
				commandTimer.Start();
				return;
			}
			if (isVideoSendingEnable && commandTimerFlag == 2)
			{
				Console.WriteLine("KOMUT GONDERILDI VIDEO GONDERILMEYE BASLANACAK.");
				commandTimer.Stop();
				commandTimer.Enabled = false;
				commandActive = false;
				waitforConformation = false;
			}
			else
			{
				Console.WriteLine("KOMUT GONDERILDI DEVAM...");
				commandTimer.Stop();
				commandTimer.Enabled = false;
			}
		}


		/* Kullanıcı tarafından Manual motor tahrik komut paketinin gönderilmesine yarayan fonksiyondur.
		 * Tıpkı manuel ayrışma gibi aynı algoritmik mantığa dayanmaktadır.
		 */
        private void manualMotorButton_Click(object sender, EventArgs e)
        {
			checkVideoActiveforCommandSending();
			gcsCommandPacket.COMMAND = (byte)Command_Types.TEST_MOTOR;
			activateCommandTimer();
		}
	}
}
