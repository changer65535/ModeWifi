function insertWDialog (parent,id,onCloseFunction,title)
{

    var dlg = document.createElement("DIV");
    dlg.id = id;
    if (typeof title !== "undefined") dlg.title = title;
    //parent.appendChild(dlg);
    $(dlg).dialog({ autoOpen: true,appendTo:parent,close:onCloseFunction,modal:true});
    return dlg;


}
function destroyWDialog (dlg)
{
    $(dlg).empty();
    $(dlg).dialog( "destroy" );
    $(dlg).remove();
}

function insertWButton (parent,id,value,name,className,funct)
{
    return insertButton(parent,id,value,name,"ui-button ui-widget ui-corner-all",funct);
}

function insertFieldSet (parent,title)
{
    var f = document.createElement("FIELDSET");
    var legend = document.createElement("LEGEND");
    legend.innerText = title;
    f.appendChild(legend);
    parent.appendChild(f);
    return f;
}
function insertWRadioButton (parent,id,value,name,label)
{
    var newLabel = document.createElement("LABEL");
    newLabel.htmlFor = id;
    newLabel.innerText = label;
    parent.appendChild(newLabel);

    var b = document.createElement("INPUT");
    b.id = id;
    b.name = name;
    b.setAttribute("type","radio");
    parent.appendChild(b);
    $(b).checkboxradio();

    return b;



}
function insertWSpinner (parent, id, min, max, start, changeFunction)
{
    var s = document.createElement("INPUT");
    s.id = id;
    s.name="value";
    parent.appendChild(s);
    var spinner = $(s).spinner(
        {
            min:min,
            max:max,

            stop: changeFunction


        });
    spinner.spinner( "value", start );
    return spinner;

}
function insertWDatePicker (parent,id,value,name)
{

    var inp = document.createElement("INPUT");

    parent.appendChild(inp);
    $(inp).datepicker(
        {
            showButtonPanel: true
        });
    $(inp).val(value);
    return inp;
}
function insertWMenu (parent,id,t)
{
    var ul = document.createElement("ul");
    ul.innerHTML = t;
    ul.id = id;
    parent.appendChild(ul);
    $(ul).css("width","200px");

    return ul;
}

function insertWMenuLI (parent,id)
{
    var li = document.createElement("li");
    var div = document.createElement("div");
    li.appendChild(div);
    parent.appendChild(li);
    li.id = id;
    return li;
}

function insertWMenuItem (parent,id,titleText,f)
{
    var li = document.createElement("li");
    var div = document.createElement("div");
    var tn = document.createTextNode(titleText);
    div.appendChild(tn);
    li.appendChild(div);
    //li.id = id;
    div.id = id;

    parent.appendChild(li);
    if (typeof f !== "undefined") $(li).on("click",f);

    return li;

}
function insertWSubmenu (parent,id,title)
{
    var sub = insertWMenuItem(parent,id,title);
    var m2 = insertWMenu(sub,"","");
    $(m2).css("box-shadow","5px 5px 5px #000000");
    return m2;
}
function insertWMenuCheckbox (parent,id,name,labelText,checked,f)
{
    var temp = insertWMenuLI(parent);
    var cb = insertCheckbox2(temp,id,name,labelText,checked,f);
    $(cb).css("width","21px");
    $(cb).css("height","21px");
    return cb;
}
var falseChar = 9898;
//var falseChar = 9675;

var trueChar = 9673;

/*
 9723	25FB	 	WHITE MEDIUM SQUARETry it
 ◼	9724	25FC	 	BLACK MEDIUM SQUARE
 */
function insertWMenuCheckbox2 (parent,id,name,labelText,checked,f)
{
    var li = document.createElement("li");
    var div = document.createElement("div");
    if (checked) labelText = String.fromCharCode(trueChar) + labelText;
    else labelText = String.fromCharCode(falseChar) + labelText;
    var tn = document.createTextNode(labelText);

    div.appendChild(tn);
    li.appendChild(div);
    li.id = id;

    parent.appendChild(li);
    if (typeof f !== "undefined") $(li).on("click",f);
    li.checked = checked;

    return li;
}

function toggleWCheck(parent)
{
    parent.checked = !parent.checked;
    var tn = parent.childNodes[0].childNodes[0];
    var s = tn.textContent;
    if (parent.checked) s = String.fromCharCode(trueChar) + s.substr(1);
    else s = String.fromCharCode(falseChar) + s.substr(1);
    tn.textContent = s;
    $(parent.parentElement).slideToggle();
    return parent.checked;
}

function drawWMenu (cMenu,x,y)
{
    $(cMenu).menu();


    if (typeof x !== "undefined")
    {
        $(cMenu).css("left",x + "px");
        $(cMenu).css("position","absolute");
        $(cMenu).css("top",y + "px");

    }
    $(cMenu).css("box-shadow","5px 5px 5px #000000");
    $(cMenu).css("z-index","9999");
    //$(cMenu).css("option","position",{collision:"flipfit"});

}

function insertWProgress(parent,id,value)
{
    var p = document.createElement("div");
    p.id = id;
    parent.appendChild(p);
    $(p).progressbar({
        value: value
    });

    return (p);


}
function setWProgressValue (p,value)
{
    $(p).progressbar("option","value",value);
}
function setWProgressColor (p,c)
{
    $(p.childNodes[0]).css("background",c);
}
function setWProgressBackground (p,c)
{
    $(p.childNodes[0]).css("background",c);
}
function setWProgressHeight (p,h)
{
    $(p).css("height",h);
}
function insertWtabCtrl(parent,id)
{
    var d = insertDiv(parent,id);
    var ul = document.createElement("ul");
    d.appendChild(ul);
    return d;
}
function insertWtabItem(tabParent,divID,text)
{
    var ul = $("#" + tabParent.id + " ul").get(0);//get the unsigned list
    var li = document.createElement("li");
    insertHREF(li,"#" + divID,text);
    ul.appendChild(li);
    var d = insertDiv(tabParent,divID);
    return d;
}
function createWtabCtrl (parent)
{
    $(parent).tabs();
}
function insertWAccordianHeader (parent,id,title)
{
    insertHeader3(parent,title);
    var d = insertDiv(parent,id);
    return d;
}


