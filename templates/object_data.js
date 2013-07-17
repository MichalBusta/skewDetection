

series = [{
                name: 'Correct Classifications',
                color: '#4572A7',
                type: 'column',
                data: [49.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6, 148.5, 216.4, 194.1, 95.6, 54.4],
                tooltip: {
                    valueSuffix: ' %'
                }
    
            }, {
                name: 'Standard Deviation',
                color: '#89A54E',
                type: 'spline',
		yAxis: 1,
                data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, 26.5, 23.3, 18.3, -1.0, 9.6],
                tooltip: {
                    valueSuffix: ''
                }
            }]

categories = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
                    'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
