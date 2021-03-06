/*
 * Tab.re
 *
 */

open Revery.UI;

open Oni_Core;

module Model = Oni_Model;

module FontAwesome = Oni_Components.FontAwesome;
module FontIcon = Oni_Components.FontIcon;

module Sneakable = Feature_Sneak.View.Sneakable;

module Theme = Feature_Theme;

module Constants = {
  include Constants;

  let minWidth = 125;
};

module Colors = Theme.Colors.Tab;

let proportion = factor =>
  float(Constants.minWidth) *. factor |> int_of_float;

module Styles = {
  open Style;

  let container =
      (
        ~mode as _,
        ~isGroupFocused,
        ~isActive,
        ~isHovered,
        ~isModified,
        ~theme,
      ) => {
    let background = {
      let unhovered =
        switch (isActive, isGroupFocused) {
        | (false, _) => Colors.inactiveBackground
        | (true, false) => Colors.unfocusedActiveBackground
        | (true, true) => Colors.activeBackground
        };

      if (isHovered) {
        let color =
          isGroupFocused
            ? Colors.unfocusedHoverBackground : Colors.hoverBackground;

        color.tryFrom(theme) |> Option.value(~default=unhovered.from(theme));
      } else {
        unhovered.from(theme);
      };
    };

    let borderTop = {
      let color =
        if (isActive) {
          background;
        } else {
          let color =
            isGroupFocused
              ? Colors.activeBorderTop : Colors.unfocusedActiveBorderTop;

          color.tryFrom(theme) |> Option.value(~default=background);
        };

      borderTop(~color, ~width=2);
    };

    let borderBottom = {
      let color = {
        let unhovered =
          (
            switch (isActive, isGroupFocused, isModified) {
            | (false, _, false) => Colors.border
            | (false, false, true) => Colors.unfocusedInactiveModifiedBorder
            | (false, true, true) => Colors.inactiveModifiedBorder
            | (true, false, true) => Colors.unfocusedActiveModifiedBorder
            | (true, true, true) => Colors.activeModifiedBorder
            | (true, false, false) => Colors.unfocusedActiveBorder
            | (true, true, false) => Colors.activeBorder
            }
          ).
            tryFrom(
            theme,
          )
          |> Option.value(~default=background);

        if (isHovered) {
          let color =
            isGroupFocused ? Colors.unfocusedHoverBorder : Colors.hoverBorder;

          color.tryFrom(theme) |> Option.value(~default=unhovered);
        } else {
          unhovered;
        };
      };

      borderBottom(~color, ~width=1);
    };

    [
      overflow(`Hidden),
      paddingHorizontal(5),
      backgroundColor(background),
      borderTop,
      borderBottom,
      height(Constants.tabHeight),
      minWidth(Constants.minWidth),
      flexDirection(`Row),
      justifyContent(`Center),
      alignItems(`Center),
    ];
  };

  let text = (~isGroupFocused, ~isActive, ~uiFont: UiFont.t, ~theme) => {
    let foreground =
      switch (isActive, isGroupFocused) {
      | (false, false) => Colors.unfocusedInactiveForeground
      | (false, true) => Colors.inactiveForeground
      | (true, false) => Colors.unfocusedActiveForeground
      | (true, true) => Colors.activeForeground
      };
    [
      width(proportion(0.80) - 10),
      textOverflow(`Ellipsis),
      fontFamily(
        isGroupFocused && isActive ? uiFont.fontFileItalic : uiFont.fontFile,
      ),
      fontSize(uiFont.fontSize),
      color(foreground.from(theme)),
      justifyContent(`Center),
      alignItems(`Center),
    ];
  };

  let icon = [
    width(32),
    height(Constants.tabHeight),
    alignItems(`Center),
    justifyContent(`Center),
  ];
};

let%component make =
              (
                ~title,
                ~isGroupFocused,
                ~isActive,
                ~isModified,
                ~onClick,
                ~onClose,
                ~theme: ColorTheme.Colors.t,
                ~uiFont: UiFont.t,
                ~mode: Vim.Mode.t,
                ~icon,
                (),
              ) => {
  let%hook (isHovered, setHovered) = Hooks.state(false);

  let fileIconView =
    switch (icon) {
    | Some((icon: IconTheme.IconDefinition.t)) =>
      <FontIcon
        fontFamily="seti.ttf"
        icon={icon.fontCharacter}
        color={icon.fontColor}
        /* TODO: Use 'weight' value from IconTheme font */
        fontSize={uiFont.fontSize *. 1.5}
      />
    | None => React.empty
    };

  let onAnyClick = (evt: NodeEvents.mouseButtonEventParams) => {
    switch (evt.button) {
    | Revery.MouseButton.BUTTON_MIDDLE => onClose()
    | Revery.MouseButton.BUTTON_LEFT => onClick()
    | _ => ()
    };
  };

  <View
    onMouseOver={_ => setHovered(_ => true)}
    onMouseOut={_ => setHovered(_ => false)}
    style={Styles.container(
      ~mode,
      ~isGroupFocused,
      ~isActive,
      ~isHovered,
      ~isModified,
      ~theme,
    )}>
    <Sneakable
      onSneak=onClick
      onAnyClick
      style=Style.[
        width(proportion(0.80)),
        flexGrow(1),
        flexDirection(`Row),
        alignItems(`Center),
        justifyContent(`Center),
      ]>
      <View style=Styles.icon> fileIconView </View>
      <Text
        style={Styles.text(~isGroupFocused, ~isActive, ~uiFont, ~theme)}
        text=title
      />
    </Sneakable>
    <Sneakable onClick=onClose style=Styles.icon>
      <FontIcon
        icon={isModified ? FontAwesome.circle : FontAwesome.times}
        color={
          isActive
            ? Colors.activeForeground.from(theme)
            : Colors.inactiveForeground.from(theme)
        }
        fontSize={isModified ? 10. : 12.}
      />
    </Sneakable>
  </View>;
};
