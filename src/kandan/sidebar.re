module Sidebar = {
  include ReactRe.Component;
  let name = "Sidebar";
  type props = {
    channel: State.channel,
    users: list State.user,
    lastVolume: float,
    me: State.user,
    menuOpen: bool,
    onUserMenuToggled: bool => ReactRe.event => unit,
    onMediaStateUpdated: State.mediaPlayerState => ReactRe.event => unit,
    onSongSelected: State.media => ReactRe.event => unit,
    onVolumeAdjusted: float => ReactRe.event => unit,
    volume: float
  };
  let findNextMedia channel offset => {
    open State;
    let idx = max 0 (min (channel.media.order + offset) (List.length channel.playlist));
    List.nth channel.playlist idx
  };
  let render {props} => {
    let {users, channel, onSongSelected, onMediaStateUpdated} = props;
    let me = List.hd users;
    let channelUsers =
      users |>
      List.filter (fun (user: State.user) => State.UserIdSet.mem State.(user.id) channel.userIds) |>
      List.sort (
        fun a b =>
          compare (String.lowercase (Utils.nameOfUser a)) (String.lowercase (Utils.nameOfUser b))
      );
    let peopleItems =
      List.map
        (
          fun (user: State.user) =>
            <li className="user" title=(Utils.nameOfUser user) key=(string_of_int user.id)>
              <img className="avatar" src=(Utils.gravatarUrl State.(user.email)) />
              <span> (ReactRe.stringToElement (Utils.nameOfUser user)) </span>
            </li>
        )
        channelUsers;
    let playlistItem (channelMedia: State.media) (media: State.media) =>
      <li
        className="user"
        title=(
                switch media.src {
                | None => ""
                | Some src => src
                }
              )
        style={
                "backgroundColor":
                  switch (media == channelMedia) {
                  | false => ""
                  | true => "#c6c9d6"
                  }
              }
        key=(string_of_int media.order)>
        <a style={"cursor": "pointer"} onClick=(fun event => onSongSelected media event)>
          (
            ReactRe.stringToElement (
              string_of_int (media.order + 1) ^
              ". " ^ (
                switch media.src {
                | None => "Unknown"
                | Some src =>
                  let split = Local_string.split src [%bs.re "/\\//"];
                  let last = split.(Array.length split - 1);
                  Local_string.decodeURI last
                }
              )
            )
          )
        </a>
      </li>;
    let playlistItems: list ReactRe.reactElement =
      List.map (playlistItem props.channel.media) props.channel.playlist;
    <aside className="sidebar">
      <div className=("header user-header" ^ (props.menuOpen ? " open-menu" : ""))>
        <a
          className="user-menu-toggle"
          href="#"
          onClick=(props.onUserMenuToggled (not props.menuOpen))>
          <img className="avatar" src=(Utils.gravatarUrl State.(me.email)) />
          <i className="icon-angle button right" style={"height": "inherit"} />
          <span> (ReactRe.stringToElement (Utils.nameOfUser me)) </span>
        </a>
        <ul className="user-menu">
          <li />
          <li> <a href="#"> (ReactRe.stringToElement "Edit Account") </a> </li>
          <li> <a href="#" rel="nofollow"> (ReactRe.stringToElement "Logout") </a> </li>
          <li> <a href="#"> (ReactRe.stringToElement "Help") </a> </li>
          <li> <a href="#"> (ReactRe.stringToElement "About Omchaya") </a> </li>
        </ul>
      </div>
      <div className="widgets">
        <div className="widget">
          <h5 className="widget-header unselectable">
            <img src="images/people_icon.png" />
            <span> (ReactRe.stringToElement "People") </span>
          </h5>
          <div className="widget-content">
            <ul className="user_list"> (ReactRe.arrayToElement (Array.of_list peopleItems)) </ul>
          </div>
        </div>
        <div className="widget">
          <h5 className="widget-header unselectable">
            <img src="images/video_icon.png" />
            <span>
              (
                ReactRe.stringToElement (
                  "Playlist" ^ (
                    switch props.channel.mediaState {
                    | Playing => " (playing)"
                    | Paused => " (paused)"
                    | NotLoaded => " (not loaded)"
                    }
                  )
                )
              )
            </span>
          </h5>
          <div className="widget-content">
            <div>
              <ul className="user_list">
                (ReactRe.arrayToElement (Array.of_list playlistItems))
              </ul>
            </div>
          </div>
          <div className="widget-action-bar">
            <div className="dropzone">
              <i
                className="fa fa-step-backward"
                onClick=(fun event => onSongSelected (findNextMedia channel (-1)) event)
                style={"cursor": "pointer"}
              />
              (ReactRe.stringToElement "  ")
              <i
                className=(
                            "fa fa-" ^ (
                              switch props.channel.mediaState {
                              | Playing => "pause"
                              | Paused => "play"
                              | NotLoaded => "none"
                              }
                            )
                          )
                onClick=(
                          fun event =>
                            onMediaStateUpdated
                              (
                                switch props.channel.mediaState {
                                | Playing => Paused
                                | Paused => Playing
                                | NotLoaded => NotLoaded
                                }
                              )
                              event
                        )
                style={"cursor": "pointer"}
              />
              (ReactRe.stringToElement "  ")
              <i
                className="fa fa-step-forward"
                onClick=(fun event => onSongSelected (findNextMedia channel 1) event)
                style={"cursor": "pointer"}
              />
              (ReactRe.stringToElement "  ")
              <i
                className="fa fa-volume-off"
                onClick=(props.onVolumeAdjusted (props.volume == 0.0 ? props.lastVolume : 0.0))
                style={"cursor": "pointer"}
              />
              (ReactRe.stringToElement "  ")
              <i
                className="fa fa-volume-down"
                onClick=(props.onVolumeAdjusted (max 0.0 (props.volume -. 0.1)))
                style={"cursor": "pointer"}
              />
              (ReactRe.stringToElement "  ")
              <i
                className="fa fa-volume-up"
                onClick=(props.onVolumeAdjusted (min 1.0 (props.volume +. 0.1)))
                style={"cursor": "pointer"}
              />
            </div>
          </div>
        </div>
        <div className="widget">
          <h5 className="widget-header unselectable">
            <img src="images/media_icon.png" />
            <span> (ReactRe.stringToElement "My Media") </span>
          </h5>
          <div className="widget-content">
            <ul className="file_list">
              <li className="file_item">
                <a href="#" target="_blank">
                  <img src="images/img_icon.png" />
                  <span> (ReactRe.stringToElement "stockholm-is-cold.jpg") </span>
                </a>
              </li>
            </ul>
          </div>
          <div className="widget-action-bar">
            <form
              action="/channels/1/attachments.json"
              id="file_upload"
              method="post"
              name="file_upload">
              <div style={"display": "inline", "padding": 0, "margin": 0} />
              <input id="channel_id_1" name="channel_id[1]" _type="hidden" />
              <input id="file" name="file" _type="file" />
              <div className="dropzone">
                (ReactRe.stringToElement "Drop file here to upload")
              </div>
            </form>
          </div>
        </div>
      </div>
    </aside>
  };
};

include ReactRe.CreateComponent Sidebar;

let createElement
    ::channel
    ::users
    ::me
    ::menuOpen
    ::onUserMenuToggled
    ::onSongSelected
    ::onMediaStateUpdated
    ::onVolumeAdjusted
    ::lastVolume
    ::volume =>
  wrapProps {
    channel,
    users,
    me,
    menuOpen,
    onUserMenuToggled,
    onSongSelected,
    onMediaStateUpdated,
    onVolumeAdjusted,
    lastVolume,
    volume
  };
