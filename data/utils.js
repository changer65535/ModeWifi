/**
 * Created by Christopher Hanger on 6/20/2017.
 */
/**
 * Created by Christopher Hanger on 10/15/2015.
 */
function setCookie(cname, cvalue, exMinutes)
{
    if (typeof exMinutes === "undefined") exMinutes = 60 * 24 * 365;
    var d = new Date();
    d.setTime(d.getTime() + (exMinutes*60*1000));
    var expires = "expires="+d.toUTCString();
    document.cookie = cname + "=" + cvalue + "; " + expires;
    return cvalue;
}

function getCookie(cname,def)
{
    if (typeof def === "undefined") def = "";
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for(var i=0; i<ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0)==' ') c = c.substring(1);
        if (c.indexOf(name) == 0)
        {
            var ret = c.substring(name.length,c.length);
            if (ret == "false") return false;
            if (ret == "true") return true;

            if (ret == "") return def;
            return ret;
        }
    }
    return def;
}

function deleteCookie(cname)
{
    document.cookie = cname + "= x;expires=Thu, 01 Jan 1970 00:00:01 GMT;";
};


function insertCanvas (parent,id,width,height,fDown,fUp)
{
    var c = document.createElement("canvas");
    c.width = width;
    c.height = height;
    c.id = id;
    parent.appendChild(c);
    c.addEventListener("mousedown",fDown,false);
    c.addEventListener("touchstart",fDown,false);

    c.addEventListener("mouseup",fUp,false);
    c.addEventListener("touchend",fUp,false);

    dc = c.getContext("2d");
    return c;

}

function insertTextBox (parent, id, value, className, maxLength, size)
{
    maxLength = typeof maxLength !== "undefined" ? maxLength : 16;
    size = typeof size !== "undefined" ? size : 16;
    var tb = document.createElement("INPUT");
    tb.value = value;
    tb.id = id;
    $(tb).addClass(className);

    tb.className = tb.className + " " + className;

    tb.maxLength = maxLength;
    tb.size = size;
    $(tb).css("max-width","100%");
    tb.type = "text";
    if (id.search("password") != -1) tb.type = "password";
    if (id.search("phone") != -1) tb.setAttribute("type", "tel");
    if ($(tb).hasClass("phone")) tb.setAttribute("type","tel");

    //button.onclick=funct;

    parent.appendChild(tb);
    return tb;
}
function insertFileDialog (parent, id, value, className)
{

    var tb = document.createElement("INPUT");
    tb.value = value;
    tb.id = id;
    tb.className = tb.className + " " + className;
    tb.type = "file";



    parent.appendChild(tb);
    return tb;
}

function insertTextBoxArea (parent, id, value, className, maxLength, rows,columns)
{
    maxLength = typeof maxLength !== "undefined" ? maxLength : 16;
    size = typeof size !== "undefined" ? size : 16;
    var tb = document.createElement("textarea");
    tb.value = value;
    tb.id = id;
    tb.className = tb.className + " " + className;

    tb.maxLength = maxLength;
    tb.rows = rows;
    tb.cols = columns;

    if (id.substring(0,8) == "password") tb.type = "password"; else tb.type = "text";
    //button.onclick=funct;

    parent.appendChild(tb);
    return tb;
}


function insertButton (parent, id, value, name, className, funct)
{
    var b = document.createElement("BUTTON");
    b.value = value;
    b.name = name;
    b.className = b.className + " " + className;
    b.type = "button";
    b.id = id;
    b.appendChild(document.createTextNode(name));
    b.onclick=funct;
    parent.appendChild(b);
    return b;
}

function isChecked (parent)
{
    if ($(parent).is(":checked")) return true;
    return false;
}


function insertCheckbox (parent,id,name,labelText,checked,f)
{
    var cb = document.createElement("input");
    cb.type = "checkbox";
    cb.onclick = f;
    cb.name = name;
    cb.id = id;
    cb.checked = checked;
    parent.appendChild(cb);
    var label = document.createElement('label')
    label.htmlFor = "id";
    label.appendChild(document.createTextNode(labelText));
    cb.className = cb.className + " clockCheck ";
    parent.appendChild(label);
    return cb;

}
function insertCheckbox2 (parent,id,name,labelText,checked,f)
{
    var cb = document.createElement("input");
    cb.type = "checkbox";
    cb.onclick = f;
    cb.name = name;
    cb.id = id;
    cb.checked = checked;
    parent.appendChild(cb);
    var label = document.createElement('label')
    label.htmlFor = "id";
    label.appendChild(document.createTextNode(labelText));
    //cb.className = cb.className + " clockCheck ";
    parent.appendChild(label);
    return cb;

}

function insertDate (parent,id,value,name)
{

    var x = document.createElement("INPUT");
    x.setAttribute("type", "date");
    if (typeof value !== "undefined") x.value = value;
    x.name = name;
    x.id = id;
    parent.appendChild(x);
    return x;
}
function insertTimeDate (parent,id,value,name)
{

    var x = document.createElement("INPUT");
    x.setAttribute("type", "datetime-local");
    if (typeof value !== "undefined") x.value = value;
    x.name = name;
    x.id = id;
    parent.appendChild(x);
}

function insertTime (parent,name,startingTime)
{
    var x = document.createElement("INPUT");
    x.setAttribute("type", "time");
    x.name = name;
    x.value = startingTime;
    parent.appendChild(x);
    return x;
}

function insertBreak(parent)
{
    if (typeof parent === "undefined") parent = document.getElementById("bodyDiv");
    parent.appendChild(document.createElement("br"));
}

function insertLine(parent)
{
    parent.appendChild(document.createElement("hr"));
}



function insertText (parent,str,lf)
{

    if (typeof lf === "undefined") lf = true;
    newNode = document.createTextNode(str);
    parent.appendChild(newNode);
    if (lf) insertBreak(parent);
    return newNode;
}
function insertBoldText (parent,str,lf)
{

    if (typeof lf === "undefined") lf = true;
    newNode = document.createElement ("B");
    newNode.appendChild(document.createTextNode(str));
    parent.appendChild(newNode);
    if (lf) insertBreak(parent);
    return newNode;
}

function insertImage (parent,fname,x,y)
{


    var img = document.createElement("img");
    img.src = fname;
    img.height = y;
    img.width = x;
    parent.appendChild(img);
    return img;

}
function insertImageButton (parent,id,fname,x,y,f)
{
    var b = insertButton(parent,id,"","","imageButton",f);
    var img = document.createElement("img");
    $(img).css("display","block");
    $(img).css("margin","auto");
    img.src = fname;
    img.height = y-10;
    img.width = x-10;
    $(b).css("padding","5px");
    $(b).css("height",y +"px");
    $(b).css("width",x +"px");
    $(b).css("border-radius","5px");
    //$(b).css("margin-left","auto");
    //$(b).css("margin-right","auto");
    //$(b).css("vertical-align","middle");

    b.appendChild(img);

    return b;

}

function insertSelect (parent, id, name, className)
{
    var sel = document.createElement("SELECT");
    sel.id = id;
    sel.className = className;

    parent.appendChild(sel);
    return sel;
}
function insertColorPicker (parent,id,value,className)
{
    var cp = document.createElement("input");
    cp.type = "color";
    cp.id = id;
    cp.value = value;
    parent.appendChild(cp);
    return cp;

}

function insertSlider (parent,id,min,max,value)
{
    var div = document.createElement("div");
    parent.appendChild(div);
    var slider = document.createElement("input");
    slider.type = "range";
    slider.min = min;
    slider.max = max;
    slider.value = value;
    slider.id = id;
    div.appendChild(slider);
    return slider;
}
function insertOption (parent,id,name)
{
    var option = document.createElement("option");
    option.id = id;
    option.value = id;
    option.innerHTML = name;
    parent.add(option);
}

function getSelectedID(sel)
{
    return $(sel).children(":selected").attr("id");
}
function setSelectedIndex (sel,index)
{
    sel.selectedIndex = index;
}

function setSelectedText (sel,text)
{



    for (var i = 0; i < sel.options.length; i++)
    {
        if (sel.options[i].text === text)
        {
            sel.selectedIndex = i;
            break;
        }
    }
}

function setSelectedID (sel,id)
{
    $(sel).val(id);
}



function insertHREFImage (parent,src,width,height,href)
{
    var a = document.createElement("A");
    var image = document.createElement("img");
    image.src = src;
    image.width = width;
    image.height = height;
    a.href = href;
    a.appendChild(image);
    parent.appendChild(a);
    return (a);
}


function insertSMSImage (parent,src,width,height,number)
{

    var href = "sms://" + number;
    var a = document.createElement("A");
    var image = document.createElement("img");
    image.src = src;
    image.width = width;
    image.height = height;
    a.href = href;
    a.appendChild(image);
    parent.appendChild(a);
    return (a);
}
function insertSMS (parent,number,text)
{
    var href = "sms:/" + number;
    var a = document.createElement("A");
    a.href = href;
    a.appendChild(document.createTextNode(text));
    parent.appendChild(a);
    return (a);
}


function insertHREF (parent,HREF,text)
{
    var a = document.createElement("A");
    a.href = HREF;
    a.appendChild(document.createTextNode(text));
    parent.appendChild(a);
    return(a);
}
function insertTel (parent,number,description)
{
    var HREF = "tel:/" + number;
    insertHREF(parent,HREF,description);
}
function insertHeader (parent,text)
{
    var h = document.createElement("h1");
    h.appendChild(document.createTextNode(text));
    parent.appendChild(h);
    return h;

}
function insertHeader3 (parent,text)
{
    var h = document.createElement("h3");
    h.appendChild(document.createTextNode(text));
    parent.appendChild(h);
    return h;

}

function insertIframe(parent,src,width,height)
{
    //var iframeContainer = document.createElement("div");
    //iframeContainer.id = "iframeContainer";
    //var innerWidth = window.innerWidth;

    //$(iframeContainer).css("left",(innerWidth - width)/2);
    //$(iframeContainer).css("height",height);

    //$(iframeContainer).css("width",width);
    //$(iframeContainer).css("height",height);


    var iframe = document.createElement("IFRAME");
    iframe.id = "iframe";

    iframe.src = src;
    $(iframe).css("width",width + "px");
    $(iframe).css("height",height + "px");

    //iframeContainer.appendChild(iframe);
    parent.appendChild(iframe);
    //insertButton(iframeContainer,"","","X","close",function (t)
    //{
    //    $("#iframeContainer").remove();
    //});
}

function insertModalIframe(parent,src,width,height)
{
    var iframeContainer = document.createElement("div");
    iframeContainer.id = "iframeContainer";
    var innerWidth = window.innerWidth;

    $(iframeContainer).css("left",(innerWidth - width)/2);
    $(iframeContainer).css("height",height);

    $(iframeContainer).css("width",width);
    $(iframeContainer).css("height",height);


    var iframe = document.createElement("IFRAME");
    iframe.id = "iframe";

    iframe.src = src;

    iframeContainer.appendChild(iframe);


    parent.appendChild(iframeContainer);
    insertButton(iframeContainer,"","","X","close",function (t)
    {
        $("#iframeContainer").remove();
    });
}

function insertDiv (parent,id)
{
    var d = document.createElement("div");
    d.id = id;
    parent.appendChild(d);
    return d;
}
function insertModalDlg(parent,id,exitFunction)
{
    var dlg = document.createElement("div");
    dlg.className += " modalDialog";

    parent.appendChild(dlg);
    var innerDlg = document.createElement("div");
    dlg.appendChild(innerDlg);
    //innerDlg.innerHTML = "<a href='#close' title='Close' class='close'>X</a>";



    innerDlg.style.position = "absolute";
    innerDlg.style.left = "150px";
    innerDlg.style.top = "0px";
    innerDlg.style.zIndex = 20000;




    $(dlg).css("opacity","1");
    $(dlg).css("pointer-events","auto");
    $(dlg).css("z-index","20000");

    insertButton(innerDlg,"","","X","close",function (t)
    {
        destroyModalDlg(t.target.parentElement);
        if (typeof exitFunction !== "undefined") exitFunction();
    });
    innerDlg.id = id;
    var windowHeight = window.height;
    $(dlg).css("height",windowHeight);
    $(dlg).css("overflow-y","auto");
    return innerDlg;
}
function setFocus (parent)
{
    parent.getElementsByTagName('input')[0].focus();
}
function alert2(text)
{
    var alertDlg = insertModalDlg(document.body,"alertDlg");
    $(alertDlg).css("width","350px");
    insertHeader (alertDlg,"Alert!");
    insertLine(alertDlg);
    insertText(alertDlg,text);
    insertBreak(alertDlg);
    var b = insertButton(alertDlg,"","","OK","",function (t)
    {
        destroyModalDlg(alertDlg);
    });
    $(b).css("float","right");
    $(b).css("width","30%");
    insertBreak(alertDlg);
    insertBreak(alertDlg);

}

function confirm2(text,onTrueFunction)
{
    var confirmDlg = insertModalDlg(document.body,"confirmDlg");
    $(confirmDlg).css("width","350px");
    insertHeader (confirmDlg,"Confirm");
    insertLine(confirmDlg);
    insertText(confirmDlg,text);
    insertBreak(confirmDlg);
    var b = insertButton(confirmDlg,"","","Cancel","",function (t)
    {
        destroyModalDlg(confirmDlg);
        return false;
    });

    var b = insertButton(confirmDlg,"","","OK","",function (f)
    {
        destroyModalDlg(confirmDlg);
        onTrueFunction();
        return true;
    });


    $(b).css("float","right");
    $(b).css("width","30%");
    insertBreak(confirmDlg);
    insertBreak(confirmDlg);

}

function confirm3(text,t,onTrueFunction)
{
    var confirmDlg = insertModalDlg(document.body,"confirmDlg");
    $(confirmDlg).css("width","350px");
    insertHeader (confirmDlg,"Confirm");
    insertLine(confirmDlg);
    insertText(confirmDlg,text);
    insertBreak(confirmDlg);
    var b = insertButton(confirmDlg,"","","Cancel","",function (t)
    {
        destroyModalDlg(confirmDlg);
        return false;
    });

    var b = insertButton(confirmDlg,"","","OK","",function (f)
    {
        destroyModalDlg(confirmDlg);
        onTrueFunction(t);
        return true;
    });


    $(b).css("float","right");
    $(b).css("width","30%");
    insertBreak(confirmDlg);
    insertBreak(confirmDlg);

}

function destroyModalDlg(innerDlg)
{

    var dlg = innerDlg.parentElement;
    $(dlg).empty();
    $(dlg).css("opacity","0");
    $(dlg).css("pointer-events","auto");
    //$(innerDlg).css("opacity","0");
    $(innerDlg).css("pointer-events","auto");
    $(dlg).css("z-index","-1");

}
function drawTitle (str,drawBack,drawIcons)
{
    if (typeof drawBack === "undefined") drawBack = false;
    if (typeof drawIcons === "undefined") drawIcons = false;
    $("#titleDiv").empty();
    var t = document.getElementById("titleDiv");
    if (drawBack) insertHREFImage(t,"../images/back-arrow.ico",42,42,"index.html?token=" + token + "&p_key=" + p_key);
    var h = document.createElement("H1");
    h.className += "title";
    h.appendChild(document.createTextNode(str));

    t.appendChild(h);
    if (drawIcons)
    {
        insertHREFImage(t,"../images/telephone-icon.png",32,32,"tel:1-828-442-5326");
        insertHREFImage(t,"../images/facebook.ico",32,32,"https://www.facebook.com/BlueRidgeHealth");
        insertHREFImage(t,"../images/twitter_icon.png",32,32,"#");
    }

}

function initializePage(ct)
{
    if (typeof ct === "undefined") ct = true;
    token = "";
    p_key = "";

    drawTitle("Wait...",true);
    //see if we are incoming from an email

    token = getParameterByName("token");
    p_key = getParameterByName("p_key");
    if (ct) checkToken(token,p_key);

}
function checkToken(token,p_key)
{
    $.post("myhomeAJAX.php",{command: "checkToken" ,token:token, p_key:p_key}, function(s)
    {
        r = $.parseJSON(s);
        if (r == "nope")
        {
            window.location = "index.html";
        }
    });

}
function clearBody ()
{
    $("#bodyDiv").empty();
    $("#menuBody").slideUp();
}

function getParameterByName(name,ret)
{
    if (typeof ret === "undefined") ret = "";
    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
    return results === null ? ret : decodeURIComponent(results[1].replace(/\+/g, " "));
}
function insertTableRow (parent,label,id,maxLength,size,typeOfInput,value)
{
    if (typeof value === "undefined") value = "";
    if (typeof typeOfInput === "undefined") typeOfInput = "text";
    var row = parent.insertRow(-1);
    row.insertCell(-1).innerHTML = label;
    var t = "";
    var td = row.insertCell(-1);
    if (typeOfInput == "text")
    {
        insertTextBox(td,id,"","",maxLength,size).value = value;

    }
    if (typeOfInput == "date") insertDate(td,id,"d");
    if (typeOfInput == "timedate") insertTimeDate(td,id,"d");

}
function merge_options(obj1,obj2)
{
    var obj3 = {};
    for (var attrname in obj1) { obj3[attrname] = obj1[attrname]; }
    for (var attrname in obj2) { obj3[attrname] = obj2[attrname]; }
    return obj3;
}

///time date utilities////////////////////////
function string2Date(str)
{
    var arr = str.split("-");
    if ((typeof arr[0] == "undefined") || (typeof arr[1] == "undefined")) return null;
    var m = arr[1];
    var y = arr[0];
    var d1 = arr[2];
    if (str.indexOf(" ") == -1)
    {
        var str2 = m + "/" + d1 + "/" + y;
        str2 = str2.replace("Eastern Standard Time","EST");
        return new Date(str2);

    }
    var arr2 = d1.split(" ");
    var d = arr2[0];
    var arr3 = arr2[1].split(":");
    var h = arr3[0];
    var mm = arr3[1];
    var s;
    if (typeof arr3[2] === "undefined") s = "00";
    else s = arr3[2];
    var str2 = m + "/" + d + "/" + y + " " + h + ":" + mm + ":" + s;
    str2 = str2.replace("Eastern Standard Time","EST");
    return new Date(str2);
}
function string2Ticks(str)
{
    return (string2Date(str).getTime());
}


function date2DateString (d)
{
    var date = d.getDate();
    if (date<10) date = "0" + date;

    var m = d.getMonth() + 1;
    if (m<10) m = "0" + m;

    var y = d.getFullYear();


    return m + "/" + date + "/" + y;
}
function date2DateTimeString (d)
{

    return (date2DateString(d) + " " + date2TimeString(d));
}
function date2DateShortTimeString (d)
{

    return (date2DateString(d) + " " + date2ShortTimeString(d));
}

function sql2str (str)
{
    if (str == "0000-00-00 00:00:00") return "";
    var d = string2Date(str);
    var date = date2DateString(d);
    if (str.indexOf(" ") == -1) return (date);
    var time = date2ShortTimeString(d);
    return (date+" "+time);

}

function sql2ShortTimeString (str)
{
    if (str == "0000-00-00 00:00:00") return "";
    var d = string2Date(str);
    var date = date2DateString(d);
    if (str.indexOf(" ") == -1) return (date);
    var time = date2ShortTimeString(d);
    return (time);

}

function sql2ShortString (str)
{
    if (str == "0000-00-00 00:00:00") return "";
    var d = string2Date(str);
    var date = date2ShortDateString(d);
    if (str.indexOf(" ") == -1) return (date);
    var time = date2ShortTimeString(d);
    return (date+" "+time);

}
function date2ShortDateString (d)
{
    var date = d.getDate();
    if (date<10) date = "0" + date;

    var m = d.getMonth() + 1;
    if (m<10) m = "0" + m;

    //var y = d.getFullYear();


    return m + "/" + date;;
}

function date2SqlDateString (d)
{
    var date = d.getDate();
    if (date<10) date = "0" + date;

    var m = d.getMonth() + 1;
    if (m<10) m = "0" + m;

    var y = d.getFullYear();


    return y + "-" + m + "-" + date;
}

function ticks2SqlDateString (t)
{

    return date2SqlTimeDateString(new Date(t));
}
function date2SqlTimeDateString (d)
{
    return date2SqlDateString(d) + date
}


function date2TimeString (d)
{
    var h = d.getHours();
    if (h<10) h = "0"+h;

    var m = d.getMinutes();
    if (m<10) m = "0"+m;

    var s = d.getSeconds();
    if (s<10) s = "0" + s;

    return h + ":" + m + ":" + s;
}
function date2SqlTimeDateString (d)
{
    return date2SqlDateString(d) + " " + date2TimeString(d);
}

function date2ShortTimeString (d)
{
    if (isNaN( d.getTime())) return " ";
    var h = d.getHours();
    if (h<10) h = "0"+h;

    var m = d.getMinutes();
    if (m<10) m = "0"+m;



    return h + ":" + m;
}
