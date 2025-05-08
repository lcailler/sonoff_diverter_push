

class relay_button {
    constructor(id_img, url, image_on,image_off,state) {
        this.id_img = id_img;
        this.url = url;
        this.state_on = state;
        this.state_on_prev = !(state);
        this.image_on=image_on;
        this.image_off=image_off;
        this.img_refresh();
        //console.log("relay button init", url );

        //this.img_refresh_cyclic();
    }
    set_state(state){
        this.state_on = state;
        this.img_refresh();
    }
    get_state(){
        return this.state_on;
    }
    toggle_state(state){
        this.state_on = !(this.get_state());
        this.img_refresh();
    }
    /*
    img_refresh_cyclic(){
        //this.state_on=!(this.state_on);
        this.img_refresh();

        window.setTimeout(this.img_refresh_cyclic.bind(this),this.timeout);
    }
    */
    img_refresh(){
        if (this.state_on_prev !=this.state_on ){
            this.state_on_prev =this.state_on;
            var img = document.getElementById(this.id_img);
            if (this.state_on){
                img.src=this.image_on;
            }else{
                img.src=this.image_off;
            }
        }
    }
}



class remote_relay_auto_button extends relay_button {

    constructor(id_img, url, image_on,image_off, image_auto_on,image_auto_off,image_unknown_status, state,timeout) {
      super(id_img, url, image_on,image_off, state);
      this.unknown_state=true;
      this.xmlhttp_get_status = new XMLHttpRequest();
      this.xmlhttp_set_status = new XMLHttpRequest();
      this.timeout = timeout;
      this.auto_on      = "?";
      this.auto_on_prev = "x";

      this.lock=false;
      //this.f_get_Status();
      //this.f_get_Status();
      //window.setTimeout(this.f_get_Status.bind(this),1000);
      this.image_auto_on  =image_auto_on;
      this.image_auto_off =image_auto_off;
      this.image_unknown_status = image_unknown_status;
      this.img_refresh_cyclic();
      this.timeout_obj="";
      //console.log("started 4", url,this.timeout );
    }
    set_auto(state){
      this.auto_on = state;
      //this.f_set_Status();
      this.img_refresh();
    }
    get_auto(){
      return this.auto_on;
    }
    toggle_auto(){
      this.lock=true;
      this.set_auto( !(this.get_auto()));
      this.f_set_Status();
      this.img_refresh();
    }
    toggle_state(state){
      if (!(this.auto_on)){
        this.lock=true;
        this.set_state( !(this.get_state()));
        this.f_set_Status();
        this.img_refresh();
      }
    }
    img_refresh(){
      //console.log(this.state_on);
      //console.log("img_refresh",this.unknown_state);
      if ((this.state_on_prev !=this.state_on)||(this.auto_on_prev !=this.auto_on)){
          var img = document.getElementById(this.id_img);
          if (this.unknown_state){
              img.src =this.image_unknown_status;
              this.auto_on_prev = !this.auto_on;
              this.state_on_prev = !this.state_on;
          }else {
              this.auto_on_prev =this.auto_on;
              this.state_on_prev =this.state_on;
              if (this.auto_on){
                if (this.state_on){
                  img.src=this.image_auto_on;
                }else{
                  img.src=this.image_auto_off;
                }
              }
              else{
                if (this.state_on){
                  img.src=this.image_on;
                }else{
                  img.src=this.image_off;
                }
              }
          }
        }
    }
    img_refresh_cyclic(){
        this.f_get_Status();
        //this.f_set_Status();
    /*
        if (this.auto_on){
        this.f_get_Status();
        } else {
        this.img_refresh();
        }
    */
        //console.log( "img_refresh_cyclic",this.auto_on, this.timeout );
        //window.setTimeout(this.img_refresh_cyclic.bind(this),this.timeout);
        this.timeout_obj = window.setTimeout(this.img_refresh_cyclic.bind(this), this.timeout);
    }
    f_onreadystatechange_get_status(){
        //console.log("f_onreadystatechange_get_status",this.xmlhttp_get_status.readyState,this.lock,this.unknown_state );
        if (this.xmlhttp_get_status.readyState == 4) {
          if(this.xmlhttp_get_status.status == 200) {
              try {
                var data=JSON.parse(this.xmlhttp_get_status.responseText);
                  //console.log("get", data );
                  this.unknown_state=false;
                  if (!this.lock){
                      this.set_auto(data.relay_channel_active_number!=0);
                      this.set_state(data.relay_switch_state==1);
                      this.img_refresh();
                  }
                  this.lock=false;
                  //this.img_refresh_cyclic();

              } catch (e) {
                  console.log("Update failed: " + e);
                  console.log(this.xmlhttp_get_status.responseText);
              }
          }

          //window.setTimeout(function(){f_get_Status(this.xmlhttp,this.url,this.timeout);}, this.timeout);
        } else {
            this.unknown_state=true;
            this.auto_on_prev = !this.auto_on;
            this.state_on_prev = !this.state_on;
            this.img_refresh();
        }

    }
    f_onreadystatechange_set_status(){
        //console.log("f_onreadystatechange_set_status",this.xmlhttp_get_status.readyState,this.lock );
        if (this.xmlhttp_set_status.readyState == 4) {
          if(this.xmlhttp_set_status.status == 200) {
              try {
                var data=JSON.parse(this.xmlhttp_set_status.responseText);
                  //console.log("set", data );
                  //if (this.get_auto() ){
                  //  this.set_state(data.relay_switch_state==1);
                  //}
                  //this.set_state(data.relay_switch_state==1);
                  //this.set_auto(data.relay_channel_active!=0);
                  this.img_refresh();
                  //this.lock=false;

              } catch (e) {
                  console.log("Update failed: " + e);
                  console.log(this.xmlhttp_set_status.responseText);
              }
          }

          //window.setTimeout(function(){f_get_Status(this.xmlhttp,this.url,this.timeout);}, this.timeout);
        }

    }
    f_get_Status() {
        //var url=this.url;
        //console.log( "get",this.url , this.xmlhttp_get_status.timeout, this.timeout );
        this.xmlhttp_get_status.onreadystatechange = this.f_onreadystatechange_get_status();
        this.xmlhttp_get_status.open(  "GET",
                            this.url,
                            true);
        this.xmlhttp_get_status.send();
    }
    f_set_Status() {
        var state_on_str="0",auto_on_str="0";
        //console.log( "set",url , this.xmlhttp_get_status.timeout );
        if (this.get_state()){state_on_str="1";}
        if (this.get_auto() ){auto_on_str ="1";}
        var url=this.url+"?set_relay_switch_state_channel_00="+state_on_str;
        url=         url+"&set_relay_channel_active_number="+auto_on_str;
        //console.log( url , state_on_str);
        this.xmlhttp_set_status.onreadystatechange = this.f_onreadystatechange_set_status();
        this.xmlhttp_set_status.open(  "GET",
                            url,
                            true);
        this.xmlhttp_set_status.send();
    }
}
