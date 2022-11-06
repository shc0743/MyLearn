namespace DownloadToolUI
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();

            bool isNewInstance = false;
            Mutex mutex = new Mutex(true, "b41f0b38-99f0-48dd-af2d-8078d0532218", out isNewInstance);
            if (!isNewInstance)
            {

                Environment.Exit(32);
            }

            _MainInstance = new Form_Main();
            _App = new Form_AppMain();
            _App.MdiParent = _MainInstance;
            _App.Show();
            _App.WindowState = FormWindowState.Maximized;
            _MainInstance.Show();
            _MainInstance.Activate();
            Application.Run();
        }

#pragma warning disable CS8618 // ���˳����캯��ʱ������Ϊ null ���ֶα�������� null ֵ���뿼������Ϊ����Ϊ null��
        internal static Form_Main _MainInstance;
        internal static Form_AppMain _App;
#pragma warning restore CS8618 // ���˳����캯��ʱ������Ϊ null ���ֶα�������� null ֵ���뿼������Ϊ����Ϊ null��

    }
}