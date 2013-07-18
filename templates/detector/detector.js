var labelType, useGradients, nativeTextSupport, animate;

(function() {
  var ua = navigator.userAgent,
      iStuff = ua.match(/iPhone/i) || ua.match(/iPad/i),
      typeOfCanvas = typeof HTMLCanvasElement,
      nativeCanvasSupport = (typeOfCanvas == 'object' || typeOfCanvas == 'function'),
      textSupport = nativeCanvasSupport 
        && (typeof document.createElement('canvas').getContext('2d').fillText == 'function');
  //I'm setting this based on the fact that ExCanvas provides text support for IE
  //and that as of today iPhone/iPad current text support is lame
  labelType = (!nativeCanvasSupport || (textSupport && !iStuff))? 'Native' : 'HTML';
  nativeTextSupport = labelType == 'Native';
  useGradients = nativeCanvasSupport;
  animate = !(iStuff || !nativeCanvasSupport);
})();

var Log = {
  elem: false,
  write: function(text){
    if (!this.elem) 
      this.elem = document.getElementById('log');
    this.elem.innerHTML = text;
    this.elem.style.left = (500 - this.elem.offsetWidth / 2) + 'px';
  }
};
var sb;

function init(){

    //end
    //init Sunburst
    sb = new $jit.Sunburst({
        //id container for the visualization
        injectInto: 'infovis',
        //Distance between levels
        levelDistance: 70,
        //Change node and edge styles such as
        //color, width and dimensions.
        Node: {
          overridable: true,
          type: useGradients? 'gradient-multipie' : 'multipie',
          span: 5
        },
        //Select canvas labels
        //'HTML', 'SVG' and 'Native' are possible options
        Label: {
          type: labelType
        },
        //Change styles when hovering and clicking nodes
        NodeStyles: {
          enable: true,
          type: 'Native',
          stylesClick: {
            'color': '#33dddd'
          },
          stylesHover: {
            'color': '#dd3333'
          }
        },
        //Add tooltips
        Tips: {
          enable: true,
          onShow: function(tip, node) {
            var html = "<div class=\"tip-title\">" + node.name + (node._depth>0? " (" + Math.round(node.data.percent*100)/100 + "%)":"") + "</div>"; 
            var data = node.data;
            if("days" in data) {
              html += "<b>Last modified:</b> " + data.days + " days ago";
            }
            if("size" in data) {
              html += "<br /><b>File size:</b> " + Math.round(data.size / 1024) + "KB";
            }
            tip.innerHTML = html;
          }
        },
        //implement event handlers
        Events: {
          enable: true,
          onClick: function(node) {
            if(!node) return;
            //Build detailed information about the file/folder
            //and place it in the right column.
            var html = "<h4>" + node.name + "</h4>", data = node.data;
            if("days" in data) {
              html += "<b>Last modified:</b> " + data.days + " days ago";
            }
            if("size" in data) {
              html += "<br /><br /><b>File size:</b> " + Math.round(data.size / 1024) + "KB";
            }
            if("description" in data) {
              html += "<br /><br /><b>Last commit was:</b><br /><pre>" + data.description + "</pre>";
            }
            $jit.id('inner-details').innerHTML = html;
            //hide tip
            sb.tips.hide();
            //rotate
            
            if(node._depth == 2 && sb.toJSON().data["$type"] == "none") {
            	var newJSON = {"children": [
								{
	            					"children": json.children[0].children[node.data.index].children,
									"data": {
										"$angularWidth": json.children[0].children[node.data.index].data.$angularWidth,
										"$color": "#00FF00",
										"correct": json.children[0].children[node.data.index].data.correct,
										"count": json.children[0].children[node.data.index].data.count,
										"variance": json.children[0].children[node.data.index].data.variance,
										"percent": json.children[0].children[node.data.index].data.percent
									},
									"id": "Correct",
									"name": "Correct"
	            				},
								{
	            					"children": json.children[1].children[node.data.index].children,
									"data": {
										"$angularWidth": json.children[1].children[node.data.index].data.$angularWidth,
										"$color": "#FF0000",
										"correct": json.children[1].children[node.data.index].data.correct,
										"count": json.children[1].children[node.data.index].data.count,
										"variance": json.children[1].children[node.data.index].data.variance,
										"percent": json.children[1].children[node.data.index].data.percent
									},
									"id": "Incorrect",
									"name": "Incorrect"
	            				}
							],
							"data": {
								"index": node.data.index
							},
							"id": json.children[0].children[node.data.index].id,
							"name": json.name+" - "+json.children[0].children[node.data.index].name};
				sb.loadJSON(newJSON);
				sb.refresh();
            } else if (node._depth == 0) {
				sb.loadJSON(json);
				sb.refresh();
				/*var nd = $jit.Graph.Util.getNode(sb, node.id);
				//var nd = sb.getNode(node.id)
				sb.rotate(nd, animate? 'animate' : 'replot', {
					duration: 1000,
					transition: $jit.Trans.Quart.easeInOut
	            });*/
            }else {
	            sb.rotate(node, animate? 'animate' : 'replot', {
	              duration: 1000,
	              transition: $jit.Trans.Quart.easeInOut
	            });
            }
          }
        },
        // Only used when Label type is 'HTML' or 'SVG'
        // Add text to the labels. 
        // This method is only triggered on label creation
        onCreateLabel: function(domElement, node){
          var labels = sb.config.Label.type,
              aw = node.getData('angularWidth');
          if (labels === 'HTML' && (node._depth < 2 || aw > 2000)) {
            domElement.innerHTML = node.name;
          } else if (labels === 'SVG' && (node._depth < 2 || aw > 2000)) {
            domElement.firstChild.appendChild(document.createTextNode(node.name));
          }
        },
        // Only used when Label type is 'HTML' or 'SVG'
        // Change node styles when labels are placed
        // or moved.
        onPlaceLabel: function(domElement, node){
          var labels = sb.config.Label.type;
          if (labels === 'SVG') {
            var fch = domElement.firstChild;
            var style = fch.style;
            style.display = '';
            style.cursor = 'pointer';
            style.fontSize = "0.8em";
            fch.setAttribute('fill', "#fff");
          } else if (labels === 'HTML') {
            var style = domElement.style;
            style.display = '';
            style.cursor = 'pointer';
            style.fontSize = "0.8em";
            style.color = "#ddd";
            var left = parseInt(style.left);
            var w = domElement.offsetWidth;
            style.left = (left - w / 2) + 'px';
          }
        }
   });
    //load JSON data.
    sb.loadJSON(json);
    //compute positions and plot.
    sb.refresh();
    //end
}
