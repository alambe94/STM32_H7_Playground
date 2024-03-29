/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef CLOUDANDWEATHERVIEWBASE_HPP
#define CLOUDANDWEATHERVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/cloudandweather_screen/CloudAndWeatherPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TiledImage.hpp>
#include <touchgfx/containers/Container.hpp>
#include <gui/containers/forecastWheelContainer.hpp>

#include <gui/containers/settingsContainer.hpp>
#include <gui/containers/wifiMenuContainer.hpp>
#include <gui/containers/wifiConnectingContainer.hpp>
#include <touchgfx/containers/buttons/Buttons.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>
class CloudAndWeatherViewBase : public touchgfx::View<CloudAndWeatherPresenter>
{
public:
    CloudAndWeatherViewBase();
    virtual ~CloudAndWeatherViewBase() {}
    virtual void setupScreen();

    /*
     * Virtual Action Handlers
     */
    virtual void settingsPressed()
    {
        // Override and implement this function in CloudAndWeather
    }

    virtual void updateClockFormat(bool value)
    {
        // Override and implement this function in CloudAndWeather
    }

    virtual void changeCity()
    {
        // Override and implement this function in CloudAndWeather
    }

    virtual void updateTemperatureUnit(bool value)
    {
        // Override and implement this function in CloudAndWeather
    }

    virtual void closeButtonPressed()
    {
        // Override and implement this function in CloudAndWeather
    }

    virtual void wifiRetryButtonPressed()
    {
        // Override and implement this function in CloudAndWeather
    }

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box backgroundBox;
    touchgfx::FadeAnimator< touchgfx::TiledImage > gradientBackground;
    touchgfx::FadeAnimator< touchgfx::TiledImage > gradientBackground2;
    touchgfx::Container rightForecast;
    forecastWheelContainer forecast3;

    touchgfx::Container centerForecast;
    forecastWheelContainer forecast2;

    touchgfx::Container leftForecast;
    forecastWheelContainer forecast1;

    settingsContainer settingsMenu;
    wifiMenuContainer wifiMenu;
    wifiConnectingContainer wifiConnecting;
    touchgfx::FadeAnimator< touchgfx::IconButtonStyle< touchgfx::ClickButtonTrigger > > settingsButton;
    touchgfx::FadeAnimator< touchgfx::IconButtonStyle< touchgfx::ClickButtonTrigger > > closeButton;
    touchgfx::Image wifiIcon;

private:

    /*
     * Callback Declarations
     */
    touchgfx::Callback<CloudAndWeatherViewBase, const touchgfx::AbstractButtonContainer&> flexButtonCallback;
    touchgfx::Callback<CloudAndWeatherViewBase, bool> settingsMenuClockModeButtonClickedCallback;
    touchgfx::Callback<CloudAndWeatherViewBase> settingsMenuBackButonClickedCallback;
    touchgfx::Callback<CloudAndWeatherViewBase> settingsMenuChangeCityCallback;
    touchgfx::Callback<CloudAndWeatherViewBase, bool> settingsMenuTemperatureUnitButtonClickedCallback;
    touchgfx::Callback<CloudAndWeatherViewBase> settingsMenuWifiMenuSelectedCallback;
    touchgfx::Callback<CloudAndWeatherViewBase> wifiConnectingWifiRetryPressedCallback;

    /*
     * Callback Handler Declarations
     */
    void flexButtonCallbackHandler(const touchgfx::AbstractButtonContainer& src);
    void settingsMenuClockModeButtonClickedCallbackHandler(bool value);
    void settingsMenuBackButonClickedCallbackHandler();
    void settingsMenuChangeCityCallbackHandler();
    void settingsMenuTemperatureUnitButtonClickedCallbackHandler(bool value);
    void settingsMenuWifiMenuSelectedCallbackHandler();
    void wifiConnectingWifiRetryPressedCallbackHandler();

};

#endif // CLOUDANDWEATHERVIEWBASE_HPP
