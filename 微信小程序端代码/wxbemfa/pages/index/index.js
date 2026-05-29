// index.js
// 获取应用实例
const app = getApp()
import mqtt from '../../utils/js/mqtt.min.js';//加载mqtt库

Page({
  data: {
    uid:"",//用户密钥，巴法云控制台获取
    ledtopic:"garbagebin01",//控制led的主题，mqtt控制台创建
    dhttopic:"garbagebin01",//传输温湿度的主题，控制台创建
    onlineTimer: null, // 在线状态检测计时器
    device_status:"离线",// 显示led是否在线的字符串，默认离线
    ledOnOff:"关闭",
    checked: false,//led的状态。默认led关闭

    ledOnOff2: '关闭',
    checked2: false,
    wendu:"",//温度值，默认为空
    shidu:"",//湿度值，默认为空
    wendu2:"",      // 垃圾桶2温度
    shidu2:"",      // 垃圾桶2湿度
    ledOnOff2:"",// 垃圾桶2开关状态

    ultrasonic1:"",    // 垃圾桶1超声波数值
    fillRate1: 0,       // 垃圾桶1满度百分比（0-100）
    fillText1:"空",    // 满度文字描述（空/半满/满）
    // 新增：超声波+满度相关（垃圾桶2）
    ultrasonic2:"",
    fillRate2: 0,
    fillText2: "空",

    ledicon:"/utils/img/lightoff.png",//显示led图标的状态。默认是关闭状态图标
    ledicon2:"/utils/img/lightoff.png",
    client: null,//mqtt客户端，默认为空
  },

  calculateFillRate(ultrasonicValue) {
    // 转数字并处理异常值
    const num = parseFloat(ultrasonicValue);
    if (isNaN(num)) return { rate: 0, text: "数据异常" };

    let rate = 0;
    let text = "";
    // 按规则计算
    if (num >= 10) {
      rate = 0;
      text = "空（0%）";
    } else if (num <=3) {
      rate = 100;
      text = "满（100%）";
    } else {
      // 线性计算：(10 - 数值)/7 * 100，取整
      rate = Math.round(((10 - num) / 7) * 100);
      text = `${rate}%`;
    }
    console.log("计算出的满度：", rate); // 打印最终计算值
    return { rate, text };
  },

  mqttConnect(){
    var that = this
    
    //MQTT连接的配置
    var options= {
      keepalive: 60, //60s ，表示心跳间隔
      clean: true, //cleanSession不保持持久会话
      protocolVersion: 4, //MQTT v3.1.1
      clientId:this.data.uid
    }
    //初始化mqtt连接
     this.data.client = mqtt.connect('wxs://bemfa.com:9504/wss',options)
     // 连接mqtt服务器
     this.data.client.on('connect', function () {
      console.log('连接服务器成功')
      //订阅dht11温湿度主题
      that.data.client.subscribe(that.data.dhttopic, function (err) {
        if (err) {
            console.log(err)
        }
      })
    })
// 接收MQTT消息的核心解析逻辑（完整修改版）
that.data.client.on('message', function (topic, message) {
  that.setData({
    device_status: "在线"
  });
  // 清除之前的超时计时器
  if (that.data.onlineTimer) {
    clearTimeout(that.data.onlineTimer);
  }
  // 30秒未收到消息 → 自动标记离线
  that.setData({
    onlineTimer: setTimeout(() => {
      that.setData({
        device_status: "离线"
      });
    }, 30000)
  });
  
  const msg = message.toString().trim();
  console.log("接收到MQTT消息：", msg);
  
  if (msg.startsWith("DATA,")) {
    const all_data_arr = msg.split(",");
    console.log("分割后数组：", all_data_arr);
    
    if (all_data_arr.length >= 6) {
      const binNum = all_data_arr[1]; // 垃圾桶编号（1/2）
      const temp = all_data_arr[2];   // 温度
      const humi = all_data_arr[3];   // 湿度
      const ultrasonic = all_data_arr[4]; // 超声波值
      const ledState = all_data_arr[5].toLowerCase(); // 硬件状态（open/close）

      // 1. 统一转换硬件状态为小程序开关的状态
      let switchChecked = false; // 开关是否选中
      let switchIcon = "/utils/img/lightoff.png"; // 默认关闭图标
      let stateText = "关闭"; // 默认文字
      
      if (ledState == "open" || ledState == "on") {
        switchChecked = true;
        switchIcon = "/utils/img/lighton.png";
        stateText = "打开";
      } else if (ledState == "close" || ledState == "off") {
        switchChecked = false;
        switchIcon = "/utils/img/lightoff.png";
        stateText = "关闭";
      }

      // 2. 计算满度（原有逻辑不变）
      const { rate, text } = that.calculateFillRate(ultrasonic);

      // 3. 按垃圾桶编号赋值（核心：同步开关状态）
      if (binNum == "1") {
        that.setData({
          // 温湿度
          wendu: temp,
          shidu: humi,
          // 开关+状态
          ledOnOff: stateText,       // 文字状态
          checked: switchChecked,    // 开关选中状态（同步硬件）
          ledicon: switchIcon,       // 图标同步
          // 满度
          ultrasonic1: ultrasonic,
          fillRate1: rate,
          fillText1: text
        }, () => {
          console.log("垃圾桶1同步完成：", {
            checked: that.data.checked,
            ledOnOff: that.data.ledOnOff
          });
        });
      } else if (binNum == "2") {
        that.setData({
          // 温湿度
          wendu2: temp,
          shidu2: humi,
          // 开关+状态
          ledOnOff2: stateText,      // 文字状态
          checked2: switchChecked,   // 开关选中状态（同步硬件）
          ledicon2: switchIcon,      // 图标同步
          // 满度
          ultrasonic2: ultrasonic,
          fillRate2: rate,
          fillText2: text
        }, () => {
          console.log("垃圾桶2同步完成：", {
            checked2: that.data.checked2,
            ledOnOff2: that.data.ledOnOff2
          });
        });
      }
    } else {
      console.error("DATA消息字段不足，格式错误！");
    }
  }
})

    //断线重连
    this.data.client.on("reconnect", function () {
      console.log("重新连接")
    });
  },

//屏幕打开时执行的函数
  onLoad() {

    //连接mqtt
    this.mqttConnect()
    //检查设备是否在线
    this.getOnline()
    //检查设备是打开还是关闭
    this.getOnOff()
    //获取服务器上现在存储的dht11数据
  },
  //控制灯的函数1，小滑块点击后执行的函数
  onChange({ detail }){
    //detail是滑块的值，检查是打开还是关闭，并更换正确图标
    this.setData({ 
      checked: detail,
     });
     if(detail == true){//如果是打开操作
      this.data.client.publish(this.data.ledtopic, 'open1')//mqtt推送on
      this.setData({ 
        ledicon: "/utils/img/lighton.png",//设置led图片为on
       });
     }else{
      this.data.client.publish(this.data.ledtopic, 'close1')//mqtt推送off
      this.setData({ 
        ledicon: "/utils/img/lightoff.png",//设置led图片为off
       });
     }
  },
  //点击led图片执行的函数
  onChange2({ detail }){
    this.setData({ 
      checked2: detail,
     });
     if(detail == true){//如果是打开操作
      this.data.client.publish(this.data.ledtopic, 'open2')//mqtt推送on
      this.setData({ 
        ledicon2: "/utils/img/lighton.png",//设置led图片为on
       });
     }else{
      this.data.client.publish(this.data.ledtopic, 'close2')//mqtt推送off
      this.setData({ 
        ledicon2: "/utils/img/lightoff.png",//设置led图片为off
       });
     }
  },
  

  getOnline(){
    var that = this
    //请求设备状态,检查设备是否在线
     //api 接口详细说明见巴法云接入文档
    wx.request({
      url: 'https://api.bemfa.com/mqtt/status/', //状态api接口，详见巴法云接入文档
      data: {
        uid: that.data.uid,
        topic: that.data.ledtopic,
      },
      header: {
        'content-type': "application/x-www-form-urlencoded"
      },
      success (res) {
        console.log(res.data)
        if(res.data.status === "online"){
          that.setData({
            device_status:"在线"
          })
        }else{
          that.setData({
            device_status:"离线"
          })
        }
        console.log(that.data.device_status)
      }
    })    
  },
  getOnOff(){
    //获取设备状态，检查设备是打开还是关闭
    //api 接口详细说明见巴法云接入文档
    var that = this
    wx.request({
      url: 'https://api.bemfa.com/api/device/v1/data/3/get/', //状态api接口，详见巴法云接入文档
      data: {
        uid: that.data.uid,
        topic: that.data.ledtopic,
        num:1
      },
      header: {
        'content-type': "application/x-www-form-urlencoded"
      },
      success (res) {
        console.log(res)
        if("undefined" != typeof res.data.data){//判断是否获取到温湿度
          console.log(res.data.data[0].msg)
          if(res.data.data[0].msg == "on"){
            that.setData({
              checked:true,
              ledOnOff:"打开",
              ledicon: "/utils/img/lighton.png",
            })
          }else{
            that.setData({
              checked:false,
              ledOnOff:"关闭",
              ledicon: "/utils/img/lightoff.png",
            })
          }
        }

      }
    })    
  },


})
