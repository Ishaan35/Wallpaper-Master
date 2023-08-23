using Wallpaper_Master_Portal.CustomControls;

namespace Wallpaper_Master_Portal
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            MenuPanel = new Panel();
            ConfigureWallpapersButton = new CustomMenuButton();
            MenuLogoPanel = new FlowLayoutPanel();
            GetWallpapersButton = new CustomMenuButton();
            MenuPanel.SuspendLayout();
            SuspendLayout();
            // 
            // MenuPanel
            // 
            MenuPanel.BackColor = Color.FromArgb(51, 52, 78);
            MenuPanel.Controls.Add(GetWallpapersButton);
            MenuPanel.Controls.Add(ConfigureWallpapersButton);
            MenuPanel.Controls.Add(MenuLogoPanel);
            MenuPanel.Dock = DockStyle.Left;
            MenuPanel.Location = new Point(0, 0);
            MenuPanel.Name = "MenuPanel";
            MenuPanel.Size = new Size(350, 1675);
            MenuPanel.TabIndex = 0;
            // 
            // ConfigureWallpapersButton
            // 
            ConfigureWallpapersButton.BackColor = Color.FromArgb(51, 52, 78);
            ConfigureWallpapersButton.Dock = DockStyle.Top;
            ConfigureWallpapersButton.FlatAppearance.BorderSize = 0;
            ConfigureWallpapersButton.FlatStyle = FlatStyle.Flat;
            ConfigureWallpapersButton.ForeColor = Color.WhiteSmoke;
            ConfigureWallpapersButton.Location = new Point(0, 200);
            ConfigureWallpapersButton.Name = "ConfigureWallpapersButton";
            ConfigureWallpapersButton.Size = new Size(350, 100);
            ConfigureWallpapersButton.TabIndex = 4;
            ConfigureWallpapersButton.Text = "Configure Wallpapers";
            ConfigureWallpapersButton.UseVisualStyleBackColor = false;
            ConfigureWallpapersButton.Click += ConfigureWallpapersButton_Click;
            // 
            // MenuLogoPanel
            // 
            MenuLogoPanel.BackColor = Color.FromArgb(38, 39, 59);
            MenuLogoPanel.Dock = DockStyle.Top;
            MenuLogoPanel.Location = new Point(0, 0);
            MenuLogoPanel.Name = "MenuLogoPanel";
            MenuLogoPanel.Size = new Size(350, 200);
            MenuLogoPanel.TabIndex = 1;
            // 
            // GetWallpapersButton
            // 
            GetWallpapersButton.BackColor = Color.FromArgb(51, 52, 78);
            GetWallpapersButton.Dock = DockStyle.Top;
            GetWallpapersButton.FlatAppearance.BorderSize = 0;
            GetWallpapersButton.FlatStyle = FlatStyle.Flat;
            GetWallpapersButton.ForeColor = Color.WhiteSmoke;
            GetWallpapersButton.Location = new Point(0, 300);
            GetWallpapersButton.Name = "GetWallpapersButton";
            GetWallpapersButton.Size = new Size(350, 100);
            GetWallpapersButton.TabIndex = 5;
            GetWallpapersButton.Text = "Get Wallpapers";
            GetWallpapersButton.UseVisualStyleBackColor = false;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(10F, 25F);
            AutoScaleMode = AutoScaleMode.Font;
            BackColor = Color.FromArgb(22, 39, 66);
            ClientSize = new Size(2488, 1675);
            Controls.Add(MenuPanel);
            ForeColor = Color.FromArgb(22, 39, 66);
            FormBorderStyle = FormBorderStyle.SizableToolWindow;
            Name = "Form1";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Form1";
            Load += Form1_Load;
            MenuPanel.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private Panel MenuPanel;
        private FlowLayoutPanel MenuLogoPanel;
        private CustomMenuButton ConfigureWallpapersButton;
        private CustomMenuButton GetWallpapersButton;
    }
}