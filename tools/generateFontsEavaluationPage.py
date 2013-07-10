import Image, ImageFont, ImageDraw
import os
from xml.dom.minidom import Document

class FontTestWriter:
    'The font test suite writer'
    
    forbidden = ["ttf-japanese", "Kacst", "Kedage", "lohit", "Mallige", "mry", "msam", "OpenSymbol", "ori", "Pothana", "Saab", "Untitled", "Vemana", "wasy", "esint", "cmex", "cmr", "Balker", "msbm", "rsfs", "cmsy", "cmmi10" ]
    
    fonts = []
    capitals = []
    
    
 
    def __init__(self):
        
        self.getFonts('/usr/share/fonts/truetype')
        self.getFonts('/home/busta/data/cvut/textspotter/fonts')
        
    
    def getFonts(self, startDir):
        for dirname, dirnames, filenames in os.walk(startDir):
            for filename in filenames:
                print os.path.join(dirname, filename)
        
                bad = False
                for  f in self.forbidden:
                    if filename.startswith(f):
                        bad = True
                if bad:
                    continue
                
                font = ImageFont.truetype(os.path.join(dirname, filename), 64)
                for  f in self.forbidden:
                    if font.getname()[0].startswith(f):
                        bad = True
                if bad:
                    continue
                
                capitals = False
                if dirname.endswith("capitals"):
                    capitals = True
                if dirname.endswith("art"):
                    continue
                
                font = ImageFont.truetype(os.path.join(dirname, filename), 64)
                self.fonts.append(font)
                self.capitals.append(capitals)
                

    def writeSentence(self, text, outputDir, font, outDoc, tagset, capital):
    
        im = Image.new("RGB", (20, 20))
        draw = ImageDraw.Draw(im)
    
        w, h = draw.textsize(text, font=font)
        im = Image.new("RGB", (w+30, h+40),  (255,)*3)
        draw = ImageDraw.Draw(im)
        draw.text((20, 20), text, font=font, fill="#000000")
  
        imageDir = outputDir
        if not os.path.exists(imageDir):
            os.mkdir( imageDir, 0777 )
        imageName = font.getname()[0] + "-" + font.getname()[1] + ".png"
        
        imageName = imageName.replace("&", "")
        imageName = imageName.replace(" ", "")
        
        imageFile = imageDir + os.sep + imageName
        if im.size[0] < 2:
            return 
        if im.size[1] < 2:
            return 
        im.save(imageFile)
        
        imageTag = outDoc.createElement("image")
        tagset.appendChild(imageTag)
        imageNameTag = outDoc.createElement("imageName")
        imageNameTag.appendChild(outDoc.createTextNode(imageName))
        imageTag.appendChild(imageNameTag)
        taggedRectangles = outDoc.createElement("taggedRectangles")
        imageTag.appendChild(taggedRectangles)
        split = text.split(" ")
        startx = 20
        starty = 20
        ws, hs = draw.textsize(" ", font=font)
        
        totals = "";
        for  s in split:
            w, h = draw.textsize(s, font=font)
            im2 = Image.new("RGB", (w+30, h+40),  (0,)*3)
            draw2 = ImageDraw.Draw(im2)
            draw2.text((0, 0), s, font=font, fill="#FFFFFF")
            bbox2 = im2.getbbox()
            if bbox2 == None:
                continue
            
            h = bbox2[3] - bbox2[1]
            w = bbox2[2] - bbox2[0]
            
            totals += s;
            w2, h2 = draw.textsize(totals, font=font)
            im2 = Image.new("RGB", (w2+30, h2+40),  (0,)*3)
            draw2 = ImageDraw.Draw(im2)
            draw2.text((20, 10), totals, font=font, fill="#FFFFFF")
            bbox3 = im2.getbbox()
            
            startx = bbox3[2] - w 
            
            taggedRectangle = outDoc.createElement("taggedRectangle")
            taggedRectangle.setAttribute("x", str(startx))
            taggedRectangle.setAttribute("y", str(starty + bbox2[1]))
            taggedRectangle.setAttribute("width", str(w))
            taggedRectangle.setAttribute("height", str(h))
            taggedRectangle.setAttribute("modelType", "1")
            
            tagg = outDoc.createElement("tag")
            nodeText = s
            if capital: 
                nodeText = nodeText.upper()
            tagg.appendChild(outDoc.createTextNode(nodeText))
            taggedRectangle.appendChild(tagg)
            
            startx = bbox3[2] + ws
            taggedRectangles.appendChild(taggedRectangle)
            totals += " ";
        
    def process(self, text, outputDir):
        
        outDoc = Document()
        tagset = outDoc.createElement("tagset")
        outDoc.appendChild(tagset)
        
        if not os.path.exists(outputDir):
            os.mkdir( outputDir, 0777 )
        
        for i in range(len(self.fonts)):
            font = self.fonts[i]
            capital = self.capitals[i]
            self.writeSentence(text, outputDir, font, outDoc, tagset, capital)
        
        annotedFile = outputDir + os.sep + "gt.xml"
        f = open(annotedFile, 'w')
        outDoc.writexml(f)
        
text = "Grumpy wizards make toxic brew for the evil Queen and Jack" 
        
writer = FontTestWriter()
writer.process(text, "/tmp/ocr")

'write modifications'

if not os.path.exists("/tmp/ocr/modifications"):
    os.mkdir( "/tmp/ocr/modifications", 0777 )
if not os.path.exists("/tmp/ocr/modifications/TextLenth"):
    os.mkdir( "/tmp/ocr/modifications/TextLenth", 0777 )
    
for i in range(4, len(text)):  
    writer.process(text[0:i], "/tmp/ocr/modifications/TextLenth/{0}".format(i))
        
