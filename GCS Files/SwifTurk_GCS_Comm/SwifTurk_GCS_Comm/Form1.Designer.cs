
namespace SwifTurk_GCS_Comm
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialStartButton = new System.Windows.Forms.Button();
            this.releaseCommandButton = new System.Windows.Forms.Button();
            this.sendVideoButton = new System.Windows.Forms.Button();
            this.serialPortComm = new System.IO.Ports.SerialPort(this.components);
            this.videoTimer = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.team_id_label = new System.Windows.Forms.Label();
            this.packet_number_label = new System.Windows.Forms.Label();
            this.temperature_label = new System.Windows.Forms.Label();
            this.pressure_label = new System.Windows.Forms.Label();
            this.descent_speed_label = new System.Windows.Forms.Label();
            this.gps_latitude_label = new System.Windows.Forms.Label();
            this.gps_longitude_label = new System.Windows.Forms.Label();
            this.gps_alt_label = new System.Windows.Forms.Label();
            this.conainer_alt_label = new System.Windows.Forms.Label();
            this.container_pres_label = new System.Windows.Forms.Label();
            this.container_gps_lat_label = new System.Windows.Forms.Label();
            this.container_gps_long_label = new System.Windows.Forms.Label();
            this.container_gps_alt_label = new System.Windows.Forms.Label();
            this.alt_difference_label = new System.Windows.Forms.Label();
            this.pitch_label = new System.Windows.Forms.Label();
            this.roll_label = new System.Windows.Forms.Label();
            this.yaw_label = new System.Windows.Forms.Label();
            this.flight_status_label = new System.Windows.Forms.Label();
            this.video_transmission_label = new System.Windows.Forms.Label();
            this.sended_video_size_label = new System.Windows.Forms.Label();
            this.altitude_label = new System.Windows.Forms.Label();
            this.commandTimer = new System.Windows.Forms.Timer(this.components);
            this.button1 = new System.Windows.Forms.Button();
            this.videoTimePassLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // serialStartButton
            // 
            this.serialStartButton.Location = new System.Drawing.Point(309, 89);
            this.serialStartButton.Name = "serialStartButton";
            this.serialStartButton.Size = new System.Drawing.Size(170, 23);
            this.serialStartButton.TabIndex = 0;
            this.serialStartButton.Text = "Communucation Start";
            this.serialStartButton.UseVisualStyleBackColor = true;
            this.serialStartButton.Click += new System.EventHandler(this.serialStartButton_Click);
            // 
            // releaseCommandButton
            // 
            this.releaseCommandButton.Location = new System.Drawing.Point(23, 134);
            this.releaseCommandButton.Name = "releaseCommandButton";
            this.releaseCommandButton.Size = new System.Drawing.Size(228, 23);
            this.releaseCommandButton.TabIndex = 1;
            this.releaseCommandButton.Text = "Release Payload Command";
            this.releaseCommandButton.UseVisualStyleBackColor = true;
            this.releaseCommandButton.Click += new System.EventHandler(this.releaseCommandButton_Click);
            // 
            // sendVideoButton
            // 
            this.sendVideoButton.Location = new System.Drawing.Point(545, 134);
            this.sendVideoButton.Name = "sendVideoButton";
            this.sendVideoButton.Size = new System.Drawing.Size(228, 23);
            this.sendVideoButton.TabIndex = 2;
            this.sendVideoButton.Text = "Start to Send Video";
            this.sendVideoButton.UseVisualStyleBackColor = true;
            this.sendVideoButton.Click += new System.EventHandler(this.sendVideoButton_Click);
            // 
            // serialPortComm
            // 
            this.serialPortComm.BaudRate = 115200;
            this.serialPortComm.PortName = "COM9";
            // 
            // videoTimer
            // 
            this.videoTimer.Interval = 200;
            this.videoTimer.Tick += new System.EventHandler(this.videoTimer_Tick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label1.Location = new System.Drawing.Point(316, 223);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(135, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Telemetry Informations";
            // 
            // team_id_label
            // 
            this.team_id_label.AutoSize = true;
            this.team_id_label.Location = new System.Drawing.Point(211, 263);
            this.team_id_label.Name = "team_id_label";
            this.team_id_label.Size = new System.Drawing.Size(57, 13);
            this.team_id_label.TabIndex = 4;
            this.team_id_label.Text = "Team ID : ";
            // 
            // packet_number_label
            // 
            this.packet_number_label.AutoSize = true;
            this.packet_number_label.Location = new System.Drawing.Point(211, 288);
            this.packet_number_label.Name = "packet_number_label";
            this.packet_number_label.Size = new System.Drawing.Size(90, 13);
            this.packet_number_label.TabIndex = 5;
            this.packet_number_label.Text = "Packet Number : ";
            // 
            // temperature_label
            // 
            this.temperature_label.AutoSize = true;
            this.temperature_label.Location = new System.Drawing.Point(211, 311);
            this.temperature_label.Name = "temperature_label";
            this.temperature_label.Size = new System.Drawing.Size(76, 13);
            this.temperature_label.TabIndex = 6;
            this.temperature_label.Text = "Temperature : ";
            // 
            // pressure_label
            // 
            this.pressure_label.AutoSize = true;
            this.pressure_label.Location = new System.Drawing.Point(211, 334);
            this.pressure_label.Name = "pressure_label";
            this.pressure_label.Size = new System.Drawing.Size(57, 13);
            this.pressure_label.TabIndex = 7;
            this.pressure_label.Text = "Pressure : ";
            // 
            // descent_speed_label
            // 
            this.descent_speed_label.AutoSize = true;
            this.descent_speed_label.Location = new System.Drawing.Point(211, 374);
            this.descent_speed_label.Name = "descent_speed_label";
            this.descent_speed_label.Size = new System.Drawing.Size(90, 13);
            this.descent_speed_label.TabIndex = 8;
            this.descent_speed_label.Text = "Descent Speed : ";
            // 
            // gps_latitude_label
            // 
            this.gps_latitude_label.AutoSize = true;
            this.gps_latitude_label.Location = new System.Drawing.Point(211, 397);
            this.gps_latitude_label.Name = "gps_latitude_label";
            this.gps_latitude_label.Size = new System.Drawing.Size(79, 13);
            this.gps_latitude_label.TabIndex = 9;
            this.gps_latitude_label.Text = "GPS Latitude : ";
            // 
            // gps_longitude_label
            // 
            this.gps_longitude_label.AutoSize = true;
            this.gps_longitude_label.Location = new System.Drawing.Point(211, 420);
            this.gps_longitude_label.Name = "gps_longitude_label";
            this.gps_longitude_label.Size = new System.Drawing.Size(91, 13);
            this.gps_longitude_label.TabIndex = 10;
            this.gps_longitude_label.Text = "GPS Longtitude : ";
            // 
            // gps_alt_label
            // 
            this.gps_alt_label.AutoSize = true;
            this.gps_alt_label.Location = new System.Drawing.Point(211, 442);
            this.gps_alt_label.Name = "gps_alt_label";
            this.gps_alt_label.Size = new System.Drawing.Size(76, 13);
            this.gps_alt_label.TabIndex = 11;
            this.gps_alt_label.Text = "GPS Altitude : ";
            // 
            // conainer_alt_label
            // 
            this.conainer_alt_label.AutoSize = true;
            this.conainer_alt_label.Location = new System.Drawing.Point(211, 464);
            this.conainer_alt_label.Name = "conainer_alt_label";
            this.conainer_alt_label.Size = new System.Drawing.Size(99, 13);
            this.conainer_alt_label.TabIndex = 12;
            this.conainer_alt_label.Text = "Container Altitude : ";
            // 
            // container_pres_label
            // 
            this.container_pres_label.AutoSize = true;
            this.container_pres_label.Location = new System.Drawing.Point(211, 485);
            this.container_pres_label.Name = "container_pres_label";
            this.container_pres_label.Size = new System.Drawing.Size(105, 13);
            this.container_pres_label.TabIndex = 13;
            this.container_pres_label.Text = "Container Pressure : ";
            // 
            // container_gps_lat_label
            // 
            this.container_gps_lat_label.AutoSize = true;
            this.container_gps_lat_label.Location = new System.Drawing.Point(211, 507);
            this.container_gps_lat_label.Name = "container_gps_lat_label";
            this.container_gps_lat_label.Size = new System.Drawing.Size(127, 13);
            this.container_gps_lat_label.TabIndex = 14;
            this.container_gps_lat_label.Text = "Container GPS Latitude : ";
            // 
            // container_gps_long_label
            // 
            this.container_gps_long_label.AutoSize = true;
            this.container_gps_long_label.Location = new System.Drawing.Point(211, 528);
            this.container_gps_long_label.Name = "container_gps_long_label";
            this.container_gps_long_label.Size = new System.Drawing.Size(139, 13);
            this.container_gps_long_label.TabIndex = 15;
            this.container_gps_long_label.Text = "Container GPS Longtitude : ";
            // 
            // container_gps_alt_label
            // 
            this.container_gps_alt_label.AutoSize = true;
            this.container_gps_alt_label.Location = new System.Drawing.Point(211, 552);
            this.container_gps_alt_label.Name = "container_gps_alt_label";
            this.container_gps_alt_label.Size = new System.Drawing.Size(124, 13);
            this.container_gps_alt_label.TabIndex = 16;
            this.container_gps_alt_label.Text = "Container GPS Altitude : ";
            // 
            // alt_difference_label
            // 
            this.alt_difference_label.AutoSize = true;
            this.alt_difference_label.Location = new System.Drawing.Point(427, 263);
            this.alt_difference_label.Name = "alt_difference_label";
            this.alt_difference_label.Size = new System.Drawing.Size(103, 13);
            this.alt_difference_label.TabIndex = 17;
            this.alt_difference_label.Text = "Altitude Difference : ";
            // 
            // pitch_label
            // 
            this.pitch_label.AutoSize = true;
            this.pitch_label.Location = new System.Drawing.Point(429, 285);
            this.pitch_label.Name = "pitch_label";
            this.pitch_label.Size = new System.Drawing.Size(40, 13);
            this.pitch_label.TabIndex = 18;
            this.pitch_label.Text = "Pitch : ";
            // 
            // roll_label
            // 
            this.roll_label.AutoSize = true;
            this.roll_label.Location = new System.Drawing.Point(429, 306);
            this.roll_label.Name = "roll_label";
            this.roll_label.Size = new System.Drawing.Size(34, 13);
            this.roll_label.TabIndex = 19;
            this.roll_label.Text = "Roll : ";
            // 
            // yaw_label
            // 
            this.yaw_label.AutoSize = true;
            this.yaw_label.Location = new System.Drawing.Point(429, 328);
            this.yaw_label.Name = "yaw_label";
            this.yaw_label.Size = new System.Drawing.Size(37, 13);
            this.yaw_label.TabIndex = 20;
            this.yaw_label.Text = "Yaw : ";
            // 
            // flight_status_label
            // 
            this.flight_status_label.AutoSize = true;
            this.flight_status_label.Location = new System.Drawing.Point(429, 351);
            this.flight_status_label.Name = "flight_status_label";
            this.flight_status_label.Size = new System.Drawing.Size(74, 13);
            this.flight_status_label.TabIndex = 21;
            this.flight_status_label.Text = "Flight Status : ";
            // 
            // video_transmission_label
            // 
            this.video_transmission_label.AutoSize = true;
            this.video_transmission_label.Location = new System.Drawing.Point(429, 376);
            this.video_transmission_label.Name = "video_transmission_label";
            this.video_transmission_label.Size = new System.Drawing.Size(140, 13);
            this.video_transmission_label.TabIndex = 22;
            this.video_transmission_label.Text = "Video Transmission Status : ";
            // 
            // sended_video_size_label
            // 
            this.sended_video_size_label.AutoSize = true;
            this.sended_video_size_label.Location = new System.Drawing.Point(611, 161);
            this.sended_video_size_label.Name = "sended_video_size_label";
            this.sended_video_size_label.Size = new System.Drawing.Size(103, 13);
            this.sended_video_size_label.TabIndex = 23;
            this.sended_video_size_label.Text = "Sended Video Size :";
            // 
            // altitude_label
            // 
            this.altitude_label.AutoSize = true;
            this.altitude_label.Location = new System.Drawing.Point(212, 355);
            this.altitude_label.Name = "altitude_label";
            this.altitude_label.Size = new System.Drawing.Size(51, 13);
            this.altitude_label.TabIndex = 24;
            this.altitude_label.Text = "Altitude : ";
            // 
            // commandTimer
            // 
            this.commandTimer.Interval = 500;
            this.commandTimer.Tick += new System.EventHandler(this.commandTimer_Tick);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(279, 134);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(228, 23);
            this.button1.TabIndex = 25;
            this.button1.Text = "Manual Motor Test Command";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.manualMotorButton_Click);
            // 
            // videoTimePassLabel
            // 
            this.videoTimePassLabel.AutoSize = true;
            this.videoTimePassLabel.Location = new System.Drawing.Point(611, 179);
            this.videoTimePassLabel.Name = "videoTimePassLabel";
            this.videoTimePassLabel.Size = new System.Drawing.Size(0, 13);
            this.videoTimePassLabel.TabIndex = 26;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(817, 623);
            this.Controls.Add(this.videoTimePassLabel);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.altitude_label);
            this.Controls.Add(this.sended_video_size_label);
            this.Controls.Add(this.video_transmission_label);
            this.Controls.Add(this.flight_status_label);
            this.Controls.Add(this.yaw_label);
            this.Controls.Add(this.roll_label);
            this.Controls.Add(this.pitch_label);
            this.Controls.Add(this.alt_difference_label);
            this.Controls.Add(this.container_gps_alt_label);
            this.Controls.Add(this.container_gps_long_label);
            this.Controls.Add(this.container_gps_lat_label);
            this.Controls.Add(this.container_pres_label);
            this.Controls.Add(this.conainer_alt_label);
            this.Controls.Add(this.gps_alt_label);
            this.Controls.Add(this.gps_longitude_label);
            this.Controls.Add(this.gps_latitude_label);
            this.Controls.Add(this.descent_speed_label);
            this.Controls.Add(this.pressure_label);
            this.Controls.Add(this.temperature_label);
            this.Controls.Add(this.packet_number_label);
            this.Controls.Add(this.team_id_label);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.sendVideoButton);
            this.Controls.Add(this.releaseCommandButton);
            this.Controls.Add(this.serialStartButton);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button serialStartButton;
        private System.Windows.Forms.Button releaseCommandButton;
        private System.Windows.Forms.Button sendVideoButton;
        private System.IO.Ports.SerialPort serialPortComm;
        public System.Windows.Forms.Timer videoTimer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label team_id_label;
        private System.Windows.Forms.Label packet_number_label;
        private System.Windows.Forms.Label temperature_label;
        private System.Windows.Forms.Label pressure_label;
        private System.Windows.Forms.Label descent_speed_label;
        private System.Windows.Forms.Label gps_latitude_label;
        private System.Windows.Forms.Label gps_longitude_label;
        private System.Windows.Forms.Label gps_alt_label;
        private System.Windows.Forms.Label conainer_alt_label;
        private System.Windows.Forms.Label container_pres_label;
        private System.Windows.Forms.Label container_gps_lat_label;
        private System.Windows.Forms.Label container_gps_long_label;
        private System.Windows.Forms.Label container_gps_alt_label;
        private System.Windows.Forms.Label alt_difference_label;
        private System.Windows.Forms.Label pitch_label;
        private System.Windows.Forms.Label roll_label;
        private System.Windows.Forms.Label yaw_label;
        private System.Windows.Forms.Label flight_status_label;
        private System.Windows.Forms.Label video_transmission_label;
        private System.Windows.Forms.Label sended_video_size_label;
        private System.Windows.Forms.Label altitude_label;
        public System.Windows.Forms.Timer commandTimer;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label videoTimePassLabel;
    }
}

