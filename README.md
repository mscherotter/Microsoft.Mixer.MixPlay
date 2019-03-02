# Microsoft.Mixer.MixPlay
Mixer.com MixPlay interactive component for UWP Apps (C++/C#/JS)

This is based on the [mixer/interactive-cpp](https://github.com/mixer/interactive-cpp) C++ SDK and projects the API to a Windows Runtime Component that any UWP app could use.

# C# app usage:
1. Add the NuGet Package Microsoft.Mixer.MixPlay to your project.
1. Add a using directive to your App class 
    using Microsoft.Mixer.MixPlay;
2. Create an instance of Interactive as a member of the App class

        private Interactive Interactive = new Interactive();
3. Add a Launch event handler to a page so that you can use its Dispatcher
	  
        Interactive.Launch += Interactive_Launch;
	  
	  ...
	     
         private async void Interactive_Launch(object sender, LaunchEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async delegate
            {
                await Launcher.LaunchUriAsync(args.Uri);
            });
        }
4. Add a StateChanged event handler to have code run once the connection is active

        Interactive.StateChanged += Interactive_StateChanged;


5. Call Interactive.StartupAsync() with parameters from the Mixer OAuth and Mixplay developer zone.  
- 
   <OAUTH_CLIENT_ID>:        the Mixer OAuth Client ID from https://mixer.com/lab/oauth
-  <MIXPLAY_INTERACTIVE_ID>: the Mixer MixPlay Version ID (found in the Code tab of the MixPlay editor)
-  <MIXPLAY_SHARE_CODE>:     the Mixer MixPlay Share Code (found in the Publish tab of the MixPlay editor)

   You can call startup either before or after you start streaming.
 
		async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
           await Interactive.StartupAsync(<OAUTH_CLIENT_ID>, <MIXPLAY_INTERACTIVE_ID>, <MIXPLAY_SHARE_CODE>);
        }

6. All events in the MixPlay are sent to the event handlers that the app handles:
- CustomEventTriggered: text box submitted
- StateChanged: connection state changed
- ErrorHandler: error handler
- MoveHandler: mouse or joystick events
- InputHandler: button pressed
- LaunchHandler: used to show OAuth dialog

8. When you want to disconnect from the Mixer MixPlay, call 

        Interactive.Disconnect();


Created by Michael S. Scherotter
