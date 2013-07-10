import Image, ImageFont, ImageDraw
import os

extractCyrilic = False

def extractFont(code, outputDir, font):
    
    str = unichr(code)
    lower = str.lower()
    if font.getname()[0].startswith("PTF") and str == lower:
        return
    if font.getname()[0].startswith("Tapeworm") and str == lower:
        return
    if font.getname()[0].startswith("LICENSE PLATE USA") and str == lower:
        return
    if font.getname()[0].startswith("Mandatory") and str == lower:
        return
    if str == "(" or str == ")" or str == "\\" or str == "*" or str == "[" or str == "]" or str == "=" or str == "%" or str == "!" or str == ":" or str == ";" or str == "?" or str == "`" or str == "^" or str == "_" or str == "-" or str == "+":
        return
    
    
    
    im = Image.new("RGB", (20, 20))
    draw = ImageDraw.Draw(im)
    
    w, h = draw.textsize(unichr(code), font=font)
    im = Image.new("RGB", (w+30, h+60),  (255,)*3)
    draw = ImageDraw.Draw(im)
    draw.text((20, 10), unichr(code), font=font, fill="#000000")
    
    im2 = Image.new("RGB", (w+30, h+60),  (0,)*3)
    draw2 = ImageDraw.Draw(im2)
    draw2.text((20, 10), unichr(code), font=font, fill="#ffffff")
  
    box = im2.getbbox()
    if type(box) == type(None):
        return  
     
    im=im.crop(box)
  
    imageDir = outputDir + os.sep + unichr(code) 
    if not os.path.exists(imageDir):
        os.mkdir( imageDir, 0777 )
    imageFile = imageDir + os.sep + font.getname()[0] + "-" + font.getname()[1] + ".png"
    if im.size[0] < 2:
        return 
    if im.size[1] < 2:
        return 
    im.save(imageFile)
    

outputDir = "/tmp/ocr"
if not os.path.exists(outputDir):
    os.mkdir( outputDir, 0777 )
    
forbidden = ["ttf-japanese", "Kacst", "Kedage", "lohit", "Mallige", "mry", "msam", "OpenSymbol", "ori", "Pothana", "Saab", "Untitled", "Vemana", "wasy", "esint", "cmex", "cmr", "Balker", "msbm", "rsfs", "cmsy",
             "Airacobra", "Alexis", "Cactus", "Concielian", "Dark Side", "Fifteen",  "the beautiful", "Big Apple", "Alien", "Alpha Flight"]
forbidden_cyrilic = ["ttf-japanese", "DejaVu Sans-ExtraLight", "DejaVu Sans Mono-Bold", "Dark Side", "Kinnari", "Liberation", "Loma",
                     "Montepetrum", "Mukti", "NanumGothic", "Norasi", "Sans Uralic", "Tlwg", "Umpush", "UnGraphic", "Waree", "Takao", "KacstBook",
                     "Sawasdee", "Bubble", "Cactus", "Caligula", "Big Apple", "Airacobra", "20.000", "Alan Den", "Alien", "FreeMono-Bold", "Alpha Beta",
                     "Alexis", "Lohit", "Untitled1", "Vemana", "wasy10", "where", "Twelve", "Tufnell", "the", "Super Ultra", "SF", "Rudiment", 
                     "Saab", "rsfs10", "Rekha", "Rachana", "Purisa", "Pothana", "Postmaster", "Phetsarath", "ori1Uni", "OpenSymbol", "Night", "Of Wildflowers", 
                     "NeverSayDie", "Alpha", "Airmole", "All", "Amalgam", "Amazon", "a song", "Babes", "BadaBoom", "Bard", "Bio-disc", "Bittersweet", "cain", 
                     "Candice", "cinnamon", "cmex10", "cmmi10", "cmr10", "cmsy10", "Concielian", "dandelion", "Data Control", "Daville", "EastSide", "Hack", 
                     "Heartbreaker", "Ghoul", "gargi", "Garuda", "Kacst", "KG", "Meera", "Mallige", "msam10", "msbm", "Palladio", "Roman", "Schoolbook", "Turntablz", 
                     "UnBatang", "UnDinaru", "UnDotum", "UnGungseo", "UnPilgi", "English Essay", "esint10", "eufm10", "EuroStyle", "Fifteen", "Chancery", "Gingersnaps", 
                     "greenbeans", "Illuminate", "Janda", "Kedage", "Khmer", "mry_", "El Wonko"]

codes = range(ord('!'), ord('}') + 1)
if extractCyrilic:
    forbidden = forbidden_cyrilic
    codes = range(0x0400, 0x04FF)

    
for dirname, dirnames, filenames in os.walk('/home/busta/data/cvut/textspotter/trainingData/TextSpotterTrainingData/forntsStage2'):
    for filename in filenames:
        print os.path.join(dirname, filename)
        
        bad = False
        for  f in forbidden:
            if filename.startswith(f):
                bad = True
        if bad:
            continue
        
        
        font = ImageFont.truetype(os.path.join(dirname, filename), 64)
        for  f in forbidden:
            if font.getname()[0].startswith(f):
                bad = True
        if bad:
            continue
        
        for code in codes:
            extractFont(code, outputDir, font)
'''
for dirname, dirnames, filenames in os.walk('/home/busta/data/cvut/textspotter/fonts'):
    for filename in filenames:
        print os.path.join(dirname, filename)
        
        bad = False
        for  f in forbidden:
            if filename.startswith(f):
                bad = True
        if bad:
            continue
        
        
        font = ImageFont.truetype(os.path.join(dirname, filename), 64)
        for  f in forbidden:
            if font.getname()[0].startswith(f):
                bad = True
        if bad:
            continue
        
        for code in codes:
            extractFont(code, outputDir, font)
'''

  