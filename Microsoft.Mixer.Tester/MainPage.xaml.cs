using Microsoft.Mixer.MixPlay;
using System;
using System.Linq;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace InteractiveUWPTester
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page, IDisposable
    {
        private Interactive Interactive = new Interactive();

        public MainPage()
        {

            this.InitializeComponent();

            Interactive.InputReceived += Interactive_InputReceived;
            Interactive.Launch += Interactive_Launch;
            Interactive.Moved += Interactive_Moved;

            Loaded += MainPage_Loaded;

            App.Current.Suspending += Current_Suspending;
        }

        private void Current_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            Interactive.Dispose();

            Interactive = null;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {

            base.OnNavigatedFrom(e);
        }

        private async void Interactive_Moved(object sender, MoveEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, delegate
            {
                ControlId.Text = $"{args.X}, {args.Y}\n {args.ParticipantId}";
            });
        }

        private async void Interactive_Launch(object sender, LaunchEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async delegate
            {
                await Launcher.LaunchUriAsync(args.Uri);
            });
        }

        private async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            //await Interactive.AuthenticateAsync();
           await Interactive.StartupAsync("e7d6de0555ee0ecafb74fded1ec36e6951eccfed75aac7d2", "330302", "1dv628xo");
            Interactive.StateChanged += Interactive_StateChanged;

        }

        private void Interactive_StateChanged(object sender, StateChangedEventArgs args)
        {
            if (args.NewState == InteractiveState.interactive_ready)
            {
                Interactive.CreateGroup("artists", "drawing");

                //var groups = await Interactive.GetGroupsAsync();

                //System.Diagnostics.Debug.WriteLine("Groups: " + string.Join(", ", from item in groups
                //                                                                  select item.Id));
            }
        }

        private async void Interactive_InputReceived(object sender, InputEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, delegate
            {
                if (args.ControlId == "Black")
                {
                    var property = Interactive.GetControlProperty($"Image1", "backgroundImage");

                    property.String = "https://i.imgur.com/ryKQ6tq.jpg";// "https://tse3.mm.bing.net/th?id=OIP.6RGX1DlxvqSevMPOWizDSAHaJ6&pid=Api/file.jpg";
                    //Interactive.SetGroup("artists", args.ParticipantId);
                }

                ControlId.Text = $"{args.ControlId} \n {args.ParticipantId}";
            });
        }

        public void Dispose()
        {
            if (Interactive != null)
            {
                Interactive.Dispose();
                Interactive = null;
            }
        }
    }
}
