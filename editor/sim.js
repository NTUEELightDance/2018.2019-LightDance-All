var N_PART = 10;
var N_DANCER = 1;
var BPM = 128.000;

var DELAY = 0.110;

var audio = document.getElementById("myAudio");
var canvas = document.getElementById('myCanvas');
var ctx = canvas.getContext('2d');
ctx.lineWidth = 3;

var BLUE = "#0000FF";
var ORANGE = "#FF9400";
var YELLOW = "#FFFF00";
var PURPLE = "#AA00FF";
var RED = "#FF0000";
var WHITE = "#FFFFFF";
var GREEN = "#00FF00";

function shadeColor2(color, percent)
{   
    var f = parseInt(color.slice(1),16),t=percent<0?0:255,p=percent<0?percent*-1:percent,R=f>>16,G=f>>8&0x00FF,B=f&0x0000FF;
    //console.log("#"+(0x1000000+(Math.round((t-R)*p)+R)*0x10000+(Math.round((t-G)*p)+G)*0x100+(Math.round((t-B)*p)+B)).toString(16).slice(1));
    return "#"+(0x1000000+(Math.round((t-R)*p)+R)*0x10000+(Math.round((t-G)*p)+G)*0x100+(Math.round((t-B)*p)+B)).toString(16).slice(1);
}

function color(c, x)
{
  //console.log(x);
  var percent = (-1) * (1.0 - (x / 255.0));
  //console.log(percent);
  return shadeColor2(c, percent);

  /*var num = parseInt(1 + 14 * x / 255.0);
  var z = (num<10 ? String.fromCharCode(0x30+num) : String.fromCharCode(0x41+num-10));
  if(c == "Blue") return "#0000" + z + z;
  if(c == "Green") return "#00" + z + z + "00";
  if(c == "Yellow") return "#" + z + z + z + z + "00";
  if(c == "Red") return "#" + z + z + "0000";*/
}

Data = JSON.parse(Data);
Pos = JSON.parse(Pos);

window.requestAnimFrame = (function(callback) {
  return window.requestAnimationFrame || window.webkitRequestAnimationFrame || window.mozRequestAnimationFrame || window.oRequestAnimationFrame || window.msRequestAnimationFrame ||
    function(callback) {
      window.setTimeout(callback, 1000 / 60);
    };
})();

function getPos(idx, time)
{
  var bx = 0, by = 0;
  var S = Pos[idx].length;

  var lb = 0, rb = S-1;
  while(lb < rb)
  {
    var mb = (lb + rb + 1) >> 1;
    if(Pos[idx][mb][0] > time)
      rb = mb - 1;
    else
      lb = mb;
  }

  var t1 = Pos[idx][lb][0];
  var x1 = Pos[idx][lb][1];
  var y1 = Pos[idx][lb][2];

  if(lb == S-1) return [x1, y1];
  
  var t2 = Pos[idx][lb+1][0];
  var x2 = Pos[idx][lb+1][1];
  var y2 = Pos[idx][lb+1][2];

  bx = x1 + (x2-x1) * (time-t1) / (t2-t1);
  by = y1 + (y2-y1) * (time-t1) / (t2-t1);

  return [bx, by];
}

function getLight(idx, part, time)
{
  var res = 0;
  var S = Data[idx][part].length;

  if(S == 0) return res;

  var lb = 0, rb = S-1;
  while(lb < rb)
  {
    var mb = (lb + rb + 1) >> 1;
    if(Data[idx][part][mb][0] > time)
      rb = mb - 1;
    else
      lb = mb;
  }

  var st = Data[idx][part][lb][0];
  var ed = Data[idx][part][lb][1];
  var ty = Data[idx][part][lb][2];
  if(st <= time && time <= ed)
  {
    if(ty == 1)
      res = 255;
    else if(ty == 2)
    {
      var x = 255 * (time-st) / (ed-st);
      res = parseInt(x);
    }
    else if(ty == 3)
    {
      var x = 255 * (ed-time) / (ed-st);
      res = parseInt(x);
    }
  }  

  return res;
}

function draw_time(time)
{
  var beats = time / 60.0 * BPM;
  var nb = parseInt(beats);
  var frac = beats - nb;
  frac = parseInt(frac*100) / 100;
  var bar = parseInt(nb / 4) + 1;
  nb = nb % 4 + 1;
  var text = bar + " - " + nb + " - " + frac;
  
  ctx.font = "20px Monospace";
  ctx.fillStyle = "#FFFFFF";
  ctx.fillText(
      text,
      0,
      canvas.height-20
      );
}

function animate(darr, canvas, ctx, startTime)
{
  // update
  //var time = ((new Date()).getTime() - startTime) / 1000;
  
  var time = audio.currentTime + DELAY;

  // clear
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  for(var i=0; i<N_DANCER; i++)
  {
    for(var j=0; j<N_PART; j++)
    {
      var res = getLight(i, j, time);
      darr[i].setLight(j, res);
      var pos = getPos(i, time);
      darr[i].setBasePos(pos[0], pos[1]);
    }
  }

  for(var i=0; i<N_DANCER; i++)
    darr[i].draw();
  draw_time(time);

  // request new frame
  requestAnimFrame(function() {
    animate(darr, canvas, ctx, startTime);
  });
}

function Dancer(id, bx, by)
{
  this.id = id;
  this.base_x = bx;
  this.base_y = by;
  this.height = 160;
  this.width = 80;
  this.light = Array(N_PART);
  for(var i=0; i<N_PART; i++)
    this.light[i] = 0;
};

Dancer.prototype.setLight = function(idx, val)
{
  this.light[idx] = val;
};

Dancer.prototype.setBasePos = function(bx, by)
{
  this.base_x = bx;
  this.base_y = by;
}

Dancer.prototype.draw = function()
{
  // Number
  var head_radius = 20;
  ctx.font = "20px sans-serif";
  ctx.fillStyle = "#FF0000";
  ctx.fillText(
      this.id,
      this.base_x + this.width / 2 - 5,
      this.base_y + head_radius - 10
      );

  // AB leg
  // B belt
  var i_width = 30;
  var i_height = 16;
  var l1_height = 30;
  var l2_height = 20;
  var l_width = 11;
  ctx.strokeStyle = color(YELLOW, this.light[0]);
  ctx.strokeRect(
      this.base_x + this.width/2 - l_width*1.35,
      this.base_y + 100 + l1_height + 5,
      l_width,
      l2_height
      );
  ctx.strokeStyle = color(YELLOW, this.light[0]);
  ctx.strokeRect(
      this.base_x + this.width/2 + l_width*0.35,
      this.base_y + 100 + l1_height + 5,
      l_width,
      l2_height
      );
  ctx.strokeStyle = color(YELLOW, this.light[1]);
  ctx.strokeRect(
      this.base_x + this.width/2 - l_width*1.35,
      this.base_y + 100,
      l_width,
      l1_height
      );
  ctx.strokeStyle = color(YELLOW, this.light[1]);
  ctx.strokeRect(
      this.base_x + this.width/2 + l_width*0.35,
      this.base_y + 100,
      l_width,
      l1_height
      );
  ctx.strokeStyle = color(GREEN, this.light[1]);
  ctx.strokeRect(
      this.base_x + this.width/2 - i_width/2,
      this.base_y + head_radius * 2 + 35,
      i_width,
      i_height
      );

  // C body
  var b1_height = 30;
  var b1_width = 11;
  ctx.strokeStyle = color(BLUE, this.light[2]);
  ctx.strokeRect(
      this.base_x + this.width/2 - b1_width*1.35,
      this.base_y + head_radius * 2,
      b1_width,
      b1_height
      );
  ctx.strokeStyle = color(BLUE, this.light[2]);
  ctx.strokeRect(
      this.base_x + this.width/2 + b1_width*0.35,
      this.base_y + head_radius * 2,
      b1_width,
      b1_height
      );

  // DEF shoulder
  var h_width = 10;
  var h1_height = 10;
  var h2_height = 15;
  var h_radius = 7;
  ctx.strokeStyle = color(GREEN, this.light[3]);
  ctx.strokeRect(
      this.base_x + h_width,
      this.base_y + head_radius*2,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color(BLUE, this.light[4]);
  ctx.strokeRect(
      this.base_x + h_width,
      this.base_y + head_radius*2 + h1_height + 5,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color(BLUE, this.light[4]);
  ctx.strokeRect(
      this.base_x + h_width,
      this.base_y + head_radius*2 + 2*h1_height + 10,
      h_width,
      h2_height
      );
  ctx.strokeStyle = color(RED, this.light[5]);
  ctx.beginPath();
  ctx.arc(
      this.base_x + h_width + h_radius - 2,
      this.base_y + head_radius*2 + 57,
      h_radius,
      0,
      Math.PI * 2
      );
  ctx.stroke();
  
  ctx.strokeStyle = color(GREEN, this.light[3]);
  ctx.strokeRect(
      this.base_x + this.width - h_width*2,
      this.base_y + head_radius*2,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color(BLUE, this.light[4]);
  ctx.strokeRect(
      this.base_x + this.width - h_width*2,
      this.base_y + head_radius*2 + h1_height + 5,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color(BLUE, this.light[4]);
  ctx.strokeRect(
      this.base_x + this.width - h_width*2,
      this.base_y + head_radius*2 + 2*h1_height + 10,
      h_width,
      h2_height
      );
  ctx.strokeStyle = color(RED, this.light[5]);
  ctx.beginPath();
  ctx.arc(
      this.base_x + this.width - h_width*2 + h_radius - 2,
      this.base_y + head_radius*2 + 57,
      h_radius,
      0,
      Math.PI * 2
      );
  ctx.stroke();

  // G glasses
  ctx.strokeStyle = color(GREEN, this.light[6]);
  ctx.strokeRect(
      this.base_x + this.width/2 - 15,
      this.base_y,
      12,
      12
      );
  ctx.strokeRect(
      this.base_x + this.width/2 + 5,
      this.base_y,
      12,
      12
      );

  // H hat
  var hat_height = 10;
  var hat_width = 30;
  ctx.strokeStyle = color(ORANGE, this.light[7]);
  ctx.strokeRect(
      this.base_x + this.width/2 - hat_width/2,
      this.base_y - 20,
      hat_width,
      hat_height
      );

  // J head tie
  ctx.strokeStyle = color(WHITE, this.light[9]);
  ctx.beginPath();
  ctx.arc(
      this.base_x + this.width / 2,
      this.base_y + head_radius - 15,
      head_radius - 3,
      0,
      Math.PI * 2
      );
  ctx.stroke();
  var b3_height = 9;
  ctx.strokeStyle = color(GREEN, this.light[9]);
  ctx.strokeRect(
      this.base_x + this.width/2 - i_width/2,
      this.base_y + head_radius * 2 - 15,
      i_width,
      b3_height
      );

  /*
  // HI body
  var b1_height = 30;
  var b2_height = 16;
  var b1_width = 30;
  ctx.strokeStyle = color("Green", this.light[7]);
  ctx.strokeRect(
      this.base_x + this.width/2 - b1_width/2,
      this.base_y + head_radius * 2,
      b1_width,
      b1_height
      );

  ctx.strokeStyle = color("Yellow", this.light[8]);
  ctx.strokeRect(
      this.base_x + this.width/2 - b1_width/2,
      this.base_y + head_radius * 2 + b1_height + 5,
      b1_width,
      b2_height
      );

  // JKLM leg

  var l1_height = 30;
  var l2_height = 20;
  var l_width = 11;
  var f_height = 6;
  var f_width = 14;
  ctx.strokeStyle = color("Blue", this.light[9]);
  ctx.strokeRect(
      this.base_x + this.width/2 - l_width*1.35,
      this.base_y + 100,
      l_width,
      l1_height
      );
  ctx.strokeStyle = color("Blue", this.light[10]);
  ctx.strokeRect(
      this.base_x + this.width/2 + l_width*0.35,
      this.base_y + 100,
      l_width,
      l1_height
      );
  ctx.strokeStyle = color("Blue", this.light[11]);
  ctx.strokeRect(
      this.base_x + this.width/2 - l_width*1.35,
      this.base_y + 100 + l1_height + 5,
      l_width,
      l2_height
      );
  ctx.strokeStyle = color("Green", this.light[11]);
  ctx.strokeRect(
      this.base_x + this.width/2 - l_width*1.35 - f_width * 0.2,
      this.base_y + 100 + l1_height + 5 + l2_height,
      f_width,
      f_height
      );

  ctx.strokeStyle = color("Blue", this.light[12]);
  ctx.strokeRect(
      this.base_x + this.width/2 + l_width*0.35,
      this.base_y + 100 + l1_height + 5,
      l_width,
      l2_height
      );
  ctx.strokeStyle = color("Green", this.light[12]);
  ctx.strokeRect(
      this.base_x + this.width/2 + l_width*0.35 + f_width * 0.0,
      this.base_y + 100 + l1_height + 5 + l2_height,
      f_width,
      f_height
      );

  // BCDEFG arm, hand

  var h_width = 10;
  var h1_height = 10;
  var h2_height = 15;
  var h_radius = 7;
  ctx.strokeStyle = color("Blue", this.light[3]);
  ctx.strokeRect(
      this.base_x + h_width,
      this.base_y + head_radius*2,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color("Yellow", this.light[3]);
  ctx.strokeRect(
      this.base_x + h_width,
      this.base_y + head_radius*2 + h1_height + 5,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color("Blue", this.light[2]);
  ctx.strokeRect(
      this.base_x + h_width,
      this.base_y + head_radius*2 + 2*h1_height + 10,
      h_width,
      h2_height
      );
  ctx.strokeStyle = color("Yellow", this.light[1]);
  ctx.beginPath();
  ctx.arc(
      this.base_x + h_width + h_radius - 2,
      this.base_y + head_radius*2 + 57,
      h_radius,
      0,
      Math.PI * 2
      );
  ctx.stroke();
  
  ctx.strokeStyle = color("Blue", this.light[4]);
  ctx.strokeRect(
      this.base_x + this.width - h_width*2,
      this.base_y + head_radius*2,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color("Yellow", this.light[4]);
  ctx.strokeRect(
      this.base_x + this.width - h_width*2,
      this.base_y + head_radius*2 + h1_height + 5,
      h_width,
      h1_height
      );
  ctx.strokeStyle = color("Blue", this.light[5]);
  ctx.strokeRect(
      this.base_x + this.width - h_width*2,
      this.base_y + head_radius*2 + 2*h1_height + 10,
      h_width,
      h2_height
      );
  ctx.strokeStyle = color("Yellow", this.light[6]);
  ctx.beginPath();
  ctx.arc(
      this.base_x + this.width - h_width*2 + h_radius - 2,
      this.base_y + head_radius*2 + 57,
      h_radius,
      0,
      Math.PI * 2
      );
  ctx.stroke();*/
};


var darr = Array(N_DANCER);
for(var i=0; i<N_DANCER; i++)
  darr[i] = new Dancer(i+1, 50+100*i, 80);

//for(var i=0; i<N_DANCER; i++)
  //darr[i].draw();

// wait one second before starting animation
setTimeout(function() {
  var startTime = (new Date()).getTime();
  animate(darr, canvas, ctx, startTime);
}, 500);
