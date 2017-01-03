* TINY Compilation to TM Code
* File: sort.tm
* Standard prelude:
  0:     LD  5,0(0) 	load maxaddress from location 0
  1:    LDA  6,0(5) 	copy gp to mp
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* -> Variable declaration
* <- Variable declaration
* -> Function Declaration (minloc)
  4:     ST  1,-10(5) 	function : store the location of func. entry
* function : unconditional jump to next declaration belongs here
* function : function body starts here
  3:    LDC  1,6(0) 	function : load function location
  6:     ST  0,-1(6) 	function : store return address
* -> param
* a
* <- param
* -> param
* low
* <- param
* -> param
* high
* <- param
* -> compound
* -> assign
* -> Id (k)
  7:    LDC  0,-7(0) 	id: load varOffset
  8:    ADD  0,6,0 	id : calculate the address
  9:    LDA  0,0(0) 	load id address
* <- Id (k)
 10:     ST  0,-8(6) 	assign : push left address
* -> Id (low)
 11:    LDC  0,-3(0) 	id: load varOffset
 12:    ADD  0,6,0 	id : calculate the address
 13:     LD  0,0(0) 	load id value
* <- Id (low)
 14:     LD  1,-8(6) 	assign : load left address
 15:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (x)
 16:    LDC  0,-6(0) 	id: load varOffset
 17:    ADD  0,6,0 	id : calculate the address
 18:    LDA  0,0(0) 	load id address
* <- Id (x)
 19:     ST  0,-8(6) 	assign : push left address
* -> Id (a)
 20:    LDC  0,-2(0) 	id: load varOffset
 21:    ADD  0,6,0 	id : load the memory address of base address of array to ac
 22:     LD  0,0,0 	id : load the base address of array to ac
 23:     ST  0,-9(6) 	id : push base address
* -> Id (low)
 24:    LDC  0,-3(0) 	id: load varOffset
 25:    ADD  0,6,0 	id : calculate the address
 26:     LD  0,0(0) 	load id value
* <- Id (low)
 27:     LD  1,-9(6) 	id : pop base address
 28:    SUB  0,1,0 	id : calculate element address with index
 29:     LD  0,0(0) 	load id value
* <- Id (a)
 30:     LD  1,-8(6) 	assign : load left address
 31:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (i)
 32:    LDC  0,-5(0) 	id: load varOffset
 33:    ADD  0,6,0 	id : calculate the address
 34:    LDA  0,0(0) 	load id address
* <- Id (i)
 35:     ST  0,-8(6) 	assign : push left address
* -> Op
* -> Id (low)
 36:    LDC  0,-3(0) 	id: load varOffset
 37:    ADD  0,6,0 	id : calculate the address
 38:     LD  0,0(0) 	load id value
* <- Id (low)
 39:     ST  0,-9(6) 	op: push left
* -> Const
 40:    LDC  0,1(0) 	load const
* <- Const
 41:     LD  1,-9(6) 	op: load left
 42:    ADD  0,1,0 	op +
* <- Op
 43:     LD  1,-8(6) 	assign : load left address
 44:     ST  0,0(1) 	assign : store value
* <- assign
* -> while
* while : jump after body comes back here
* -> Op
* -> Id (i)
 45:    LDC  0,-5(0) 	id: load varOffset
 46:    ADD  0,6,0 	id : calculate the address
 47:     LD  0,0(0) 	load id value
* <- Id (i)
 48:     ST  0,-8(6) 	op: push left
* -> Id (high)
 49:    LDC  0,-4(0) 	id: load varOffset
 50:    ADD  0,6,0 	id : calculate the address
 51:     LD  0,0(0) 	load id value
* <- Id (high)
 52:     LD  1,-8(6) 	op: load left
 53:    SUB  0,1,0 	op <
 54:    JLT  0,2(7) 	br if true
 55:    LDC  0,0(0) 	false case
 56:    LDA  7,1(7) 	unconditional jmp
 57:    LDC  0,1(0) 	true case
* <- Op
* while : jump to end belongs here
* -> compound
* -> if
* -> Op
* -> Id (a)
 59:    LDC  0,-2(0) 	id: load varOffset
 60:    ADD  0,6,0 	id : load the memory address of base address of array to ac
 61:     LD  0,0,0 	id : load the base address of array to ac
 62:     ST  0,-8(6) 	id : push base address
* -> Id (i)
 63:    LDC  0,-5(0) 	id: load varOffset
 64:    ADD  0,6,0 	id : calculate the address
 65:     LD  0,0(0) 	load id value
* <- Id (i)
 66:     LD  1,-8(6) 	id : pop base address
 67:    SUB  0,1,0 	id : calculate element address with index
 68:     LD  0,0(0) 	load id value
* <- Id (a)
 69:     ST  0,-8(6) 	op: push left
* -> Id (x)
 70:    LDC  0,-6(0) 	id: load varOffset
 71:    ADD  0,6,0 	id : calculate the address
 72:     LD  0,0(0) 	load id value
* <- Id (x)
 73:     LD  1,-8(6) 	op: load left
 74:    SUB  0,1,0 	op <
 75:    JLT  0,2(7) 	br if true
 76:    LDC  0,0(0) 	false case
 77:    LDA  7,1(7) 	unconditional jmp
 78:    LDC  0,1(0) 	true case
* <- Op
* if: jump to else belongs here
* -> compound
* -> assign
* -> Id (x)
 80:    LDC  0,-6(0) 	id: load varOffset
 81:    ADD  0,6,0 	id : calculate the address
 82:    LDA  0,0(0) 	load id address
* <- Id (x)
 83:     ST  0,-8(6) 	assign : push left address
* -> Id (a)
 84:    LDC  0,-2(0) 	id: load varOffset
 85:    ADD  0,6,0 	id : load the memory address of base address of array to ac
 86:     LD  0,0,0 	id : load the base address of array to ac
 87:     ST  0,-9(6) 	id : push base address
* -> Id (i)
 88:    LDC  0,-5(0) 	id: load varOffset
 89:    ADD  0,6,0 	id : calculate the address
 90:     LD  0,0(0) 	load id value
* <- Id (i)
 91:     LD  1,-9(6) 	id : pop base address
 92:    SUB  0,1,0 	id : calculate element address with index
 93:     LD  0,0(0) 	load id value
* <- Id (a)
 94:     LD  1,-8(6) 	assign : load left address
 95:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (k)
 96:    LDC  0,-7(0) 	id: load varOffset
 97:    ADD  0,6,0 	id : calculate the address
 98:    LDA  0,0(0) 	load id address
* <- Id (k)
 99:     ST  0,-8(6) 	assign : push left address
* -> Id (i)
100:    LDC  0,-5(0) 	id: load varOffset
101:    ADD  0,6,0 	id : calculate the address
102:     LD  0,0(0) 	load id value
* <- Id (i)
103:     LD  1,-8(6) 	assign : load left address
104:     ST  0,0(1) 	assign : store value
* <- assign
* <- compound
* if: jump to end belongs here
 79:    JEQ  0,26(7) 	if: jmp to else
105:    LDA  7,0(7) 	jmp to end
* <- if
* -> assign
* -> Id (i)
106:    LDC  0,-5(0) 	id: load varOffset
107:    ADD  0,6,0 	id : calculate the address
108:    LDA  0,0(0) 	load id address
* <- Id (i)
109:     ST  0,-8(6) 	assign : push left address
* -> Op
* -> Id (i)
110:    LDC  0,-5(0) 	id: load varOffset
111:    ADD  0,6,0 	id : calculate the address
112:     LD  0,0(0) 	load id value
* <- Id (i)
113:     ST  0,-9(6) 	op: push left
* -> Const
114:    LDC  0,1(0) 	load const
* <- Const
115:     LD  1,-9(6) 	op: load left
116:    ADD  0,1,0 	op +
* <- Op
117:     LD  1,-8(6) 	assign : load left address
118:     ST  0,0(1) 	assign : store value
* <- assign
* <- compound
119:    LDA  7,-75(7) 	while : jmp back to test
 58:    JEQ  0,61(7) 	while : jmp to end
* <- while
* -> return
* -> Id (k)
120:    LDC  0,-7(0) 	id: load varOffset
121:    ADD  0,6,0 	id : calculate the address
122:     LD  0,0(0) 	load id value
* <- Id (k)
123:     LD  7,-1(6) 	return : to caller
* <- return
* <- compound
124:     LD  7,-1(6) 	func : load pc with return address
  5:    LDA  7,119(7) 	func : unconditional jump to next declaration
* <- Function Declaration (minloc)
* -> Function Declaration (sort)
126:     ST  1,-11(5) 	function : store the location of func. entry
* function : unconditional jump to next declaration belongs here
* function : function body starts here
125:    LDC  1,128(0) 	function : load function location
128:     ST  0,-1(6) 	function : store return address
* -> param
* a
* <- param
* -> param
* low
* <- param
* -> param
* high
* <- param
* -> compound
* -> assign
* -> Id (i)
129:    LDC  0,-5(0) 	id: load varOffset
130:    ADD  0,6,0 	id : calculate the address
131:    LDA  0,0(0) 	load id address
* <- Id (i)
132:     ST  0,-7(6) 	assign : push left address
* -> Id (low)
133:    LDC  0,-3(0) 	id: load varOffset
134:    ADD  0,6,0 	id : calculate the address
135:     LD  0,0(0) 	load id value
* <- Id (low)
136:     LD  1,-7(6) 	assign : load left address
137:     ST  0,0(1) 	assign : store value
* <- assign
* -> while
* while : jump after body comes back here
* -> Op
* -> Id (i)
138:    LDC  0,-5(0) 	id: load varOffset
139:    ADD  0,6,0 	id : calculate the address
140:     LD  0,0(0) 	load id value
* <- Id (i)
141:     ST  0,-7(6) 	op: push left
* -> Op
* -> Id (high)
142:    LDC  0,-4(0) 	id: load varOffset
143:    ADD  0,6,0 	id : calculate the address
144:     LD  0,0(0) 	load id value
* <- Id (high)
145:     ST  0,-8(6) 	op: push left
* -> Const
146:    LDC  0,1(0) 	load const
* <- Const
147:     LD  1,-8(6) 	op: load left
148:    SUB  0,1,0 	op -
* <- Op
149:     LD  1,-7(6) 	op: load left
150:    SUB  0,1,0 	op <
151:    JLT  0,2(7) 	br if true
152:    LDC  0,0(0) 	false case
153:    LDA  7,1(7) 	unconditional jmp
154:    LDC  0,1(0) 	true case
* <- Op
* while : jump to end belongs here
* -> compound
* -> assign
* -> Id (k)
156:    LDC  0,-6(0) 	id: load varOffset
157:    ADD  0,6,0 	id : calculate the address
158:    LDA  0,0(0) 	load id address
* <- Id (k)
159:     ST  0,-8(6) 	assign : push left address
* -> Call (minloc)
* -> Id (a)
160:    LDC  0,-2(0) 	id: load varOffset
161:    ADD  0,6,0 	id : load the memory address of base address of array to ac
162:     LD  0,0,0 	id : load the base address of array to ac
163:     ST  0,-9(6) 	id : push base address
164:    LDC  0,0(0) 	id : no need for index
165:     LD  1,-9(6) 	id : pop base address
166:    SUB  0,1,0 	id : calculate element address with index
167:    LDA  0,0(0) 	load id address
* <- Id (a)
168:     ST  0,-11(6) 	call : push argument
* -> Id (i)
169:    LDC  0,-5(0) 	id: load varOffset
170:    ADD  0,6,0 	id : calculate the address
171:     LD  0,0(0) 	load id value
* <- Id (i)
172:     ST  0,-12(6) 	call : push argument
* -> Id (high)
173:    LDC  0,-4(0) 	id: load varOffset
174:    ADD  0,6,0 	id : calculate the address
175:     LD  0,0(0) 	load id value
* <- Id (high)
176:     ST  0,-13(6) 	call : push argument
177:     ST  6,-9(6) 	call : store current mp
178:    LDA  6,-9(6) 	call : push new frame
179:    LDA  0,1(7) 	call : save return in ac
180:     LD  7,-10(5) 	call : relative jump to function entry
181:     LD  6,0(6) 	call : pop current frame
* <- Call (minloc)
182:     LD  1,-8(6) 	assign : load left address
183:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (t)
184:    LDC  0,-7(0) 	id: load varOffset
185:    ADD  0,6,0 	id : calculate the address
186:    LDA  0,0(0) 	load id address
* <- Id (t)
187:     ST  0,-8(6) 	assign : push left address
* -> Id (a)
188:    LDC  0,-2(0) 	id: load varOffset
189:    ADD  0,6,0 	id : load the memory address of base address of array to ac
190:     LD  0,0,0 	id : load the base address of array to ac
191:     ST  0,-9(6) 	id : push base address
* -> Id (k)
192:    LDC  0,-6(0) 	id: load varOffset
193:    ADD  0,6,0 	id : calculate the address
194:     LD  0,0(0) 	load id value
* <- Id (k)
195:     LD  1,-9(6) 	id : pop base address
196:    SUB  0,1,0 	id : calculate element address with index
197:     LD  0,0(0) 	load id value
* <- Id (a)
198:     LD  1,-8(6) 	assign : load left address
199:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (a)
200:    LDC  0,-2(0) 	id: load varOffset
201:    ADD  0,6,0 	id : load the memory address of base address of array to ac
202:     LD  0,0,0 	id : load the base address of array to ac
203:     ST  0,-8(6) 	id : push base address
* -> Id (k)
204:    LDC  0,-6(0) 	id: load varOffset
205:    ADD  0,6,0 	id : calculate the address
206:     LD  0,0(0) 	load id value
* <- Id (k)
207:     LD  1,-8(6) 	id : pop base address
208:    SUB  0,1,0 	id : calculate element address with index
209:    LDA  0,0(0) 	load id address
* <- Id (a)
210:     ST  0,-8(6) 	assign : push left address
* -> Id (a)
211:    LDC  0,-2(0) 	id: load varOffset
212:    ADD  0,6,0 	id : load the memory address of base address of array to ac
213:     LD  0,0,0 	id : load the base address of array to ac
214:     ST  0,-9(6) 	id : push base address
* -> Id (i)
215:    LDC  0,-5(0) 	id: load varOffset
216:    ADD  0,6,0 	id : calculate the address
217:     LD  0,0(0) 	load id value
* <- Id (i)
218:     LD  1,-9(6) 	id : pop base address
219:    SUB  0,1,0 	id : calculate element address with index
220:     LD  0,0(0) 	load id value
* <- Id (a)
221:     LD  1,-8(6) 	assign : load left address
222:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (a)
223:    LDC  0,-2(0) 	id: load varOffset
224:    ADD  0,6,0 	id : load the memory address of base address of array to ac
225:     LD  0,0,0 	id : load the base address of array to ac
226:     ST  0,-8(6) 	id : push base address
* -> Id (i)
227:    LDC  0,-5(0) 	id: load varOffset
228:    ADD  0,6,0 	id : calculate the address
229:     LD  0,0(0) 	load id value
* <- Id (i)
230:     LD  1,-8(6) 	id : pop base address
231:    SUB  0,1,0 	id : calculate element address with index
232:    LDA  0,0(0) 	load id address
* <- Id (a)
233:     ST  0,-8(6) 	assign : push left address
* -> Id (t)
234:    LDC  0,-7(0) 	id: load varOffset
235:    ADD  0,6,0 	id : calculate the address
236:     LD  0,0(0) 	load id value
* <- Id (t)
237:     LD  1,-8(6) 	assign : load left address
238:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (i)
239:    LDC  0,-5(0) 	id: load varOffset
240:    ADD  0,6,0 	id : calculate the address
241:    LDA  0,0(0) 	load id address
* <- Id (i)
242:     ST  0,-8(6) 	assign : push left address
* -> Op
* -> Id (i)
243:    LDC  0,-5(0) 	id: load varOffset
244:    ADD  0,6,0 	id : calculate the address
245:     LD  0,0(0) 	load id value
* <- Id (i)
246:     ST  0,-9(6) 	op: push left
* -> Const
247:    LDC  0,1(0) 	load const
* <- Const
248:     LD  1,-9(6) 	op: load left
249:    ADD  0,1,0 	op +
* <- Op
250:     LD  1,-8(6) 	assign : load left address
251:     ST  0,0(1) 	assign : store value
* <- assign
* <- compound
252:    LDA  7,-115(7) 	while : jmp back to test
155:    JEQ  0,97(7) 	while : jmp to end
* <- while
* <- compound
253:     LD  7,-1(6) 	func : load pc with return address
127:    LDA  7,126(7) 	func : unconditional jump to next declaration
* <- Function Declaration (sort)
* -> Function Declaration (main)
255:     ST  1,-12(5) 	function : store the location of func. entry
* function : unconditional jump to next declaration belongs here
* function : function body starts here
254:    LDC  1,257(0) 	function : load function location
257:     ST  0,-1(6) 	function : store return address
* -> compound
* -> assign
* -> Id (i)
258:    LDC  0,-2(0) 	id: load varOffset
259:    ADD  0,6,0 	id : calculate the address
260:    LDA  0,0(0) 	load id address
* <- Id (i)
261:     ST  0,-3(6) 	assign : push left address
* -> Const
262:    LDC  0,0(0) 	load const
* <- Const
263:     LD  1,-3(6) 	assign : load left address
264:     ST  0,0(1) 	assign : store value
* <- assign
* -> while
* while : jump after body comes back here
* -> Op
* -> Id (i)
265:    LDC  0,-2(0) 	id: load varOffset
266:    ADD  0,6,0 	id : calculate the address
267:     LD  0,0(0) 	load id value
* <- Id (i)
268:     ST  0,-3(6) 	op: push left
* -> Const
269:    LDC  0,10(0) 	load const
* <- Const
270:     LD  1,-3(6) 	op: load left
271:    SUB  0,1,0 	op <
272:    JLT  0,2(7) 	br if true
273:    LDC  0,0(0) 	false case
274:    LDA  7,1(7) 	unconditional jmp
275:    LDC  0,1(0) 	true case
* <- Op
* while : jump to end belongs here
* -> compound
* -> assign
* -> Id (x)
277:    LDC  0,0(0) 	id: load varOffset
278:    ADD  0,5,0 	id : calculate the address
279:     ST  0,-3(6) 	id : push base address
* -> Id (i)
280:    LDC  0,-2(0) 	id: load varOffset
281:    ADD  0,6,0 	id : calculate the address
282:     LD  0,0(0) 	load id value
* <- Id (i)
283:     LD  1,-3(6) 	id : pop base address
284:    SUB  0,1,0 	id : calculate element address with index
285:    LDA  0,0(0) 	load id address
* <- Id (x)
286:     ST  0,-3(6) 	assign : push left address
* -> Call (input)
287:     IN  0,0,0 	read integer value
* <- Call (input)
288:     LD  1,-3(6) 	assign : load left address
289:     ST  0,0(1) 	assign : store value
* <- assign
* -> assign
* -> Id (i)
290:    LDC  0,-2(0) 	id: load varOffset
291:    ADD  0,6,0 	id : calculate the address
292:    LDA  0,0(0) 	load id address
* <- Id (i)
293:     ST  0,-3(6) 	assign : push left address
* -> Op
* -> Id (i)
294:    LDC  0,-2(0) 	id: load varOffset
295:    ADD  0,6,0 	id : calculate the address
296:     LD  0,0(0) 	load id value
* <- Id (i)
297:     ST  0,-4(6) 	op: push left
* -> Const
298:    LDC  0,1(0) 	load const
* <- Const
299:     LD  1,-4(6) 	op: load left
300:    ADD  0,1,0 	op +
* <- Op
301:     LD  1,-3(6) 	assign : load left address
302:     ST  0,0(1) 	assign : store value
* <- assign
* <- compound
303:    LDA  7,-39(7) 	while : jmp back to test
276:    JEQ  0,27(7) 	while : jmp to end
* <- while
* -> Call (sort)
* -> Id (x)
304:    LDC  0,0(0) 	id: load varOffset
305:    ADD  0,5,0 	id : calculate the address
306:     ST  0,-3(6) 	id : push base address
307:    LDC  0,0(0) 	id : no need for index
308:     LD  1,-3(6) 	id : pop base address
309:    SUB  0,1,0 	id : calculate element address with index
310:    LDA  0,0(0) 	load id address
* <- Id (x)
311:     ST  0,-5(6) 	call : push argument
* -> Const
312:    LDC  0,0(0) 	load const
* <- Const
313:     ST  0,-6(6) 	call : push argument
* -> Const
314:    LDC  0,10(0) 	load const
* <- Const
315:     ST  0,-7(6) 	call : push argument
316:     ST  6,-3(6) 	call : store current mp
317:    LDA  6,-3(6) 	call : push new frame
318:    LDA  0,1(7) 	call : save return in ac
319:     LD  7,-11(5) 	call : relative jump to function entry
320:     LD  6,0(6) 	call : pop current frame
* <- Call (sort)
* -> assign
* -> Id (i)
321:    LDC  0,-2(0) 	id: load varOffset
322:    ADD  0,6,0 	id : calculate the address
323:    LDA  0,0(0) 	load id address
* <- Id (i)
324:     ST  0,-3(6) 	assign : push left address
* -> Const
325:    LDC  0,0(0) 	load const
* <- Const
326:     LD  1,-3(6) 	assign : load left address
327:     ST  0,0(1) 	assign : store value
* <- assign
* -> while
* while : jump after body comes back here
* -> Op
* -> Id (i)
328:    LDC  0,-2(0) 	id: load varOffset
329:    ADD  0,6,0 	id : calculate the address
330:     LD  0,0(0) 	load id value
* <- Id (i)
331:     ST  0,-3(6) 	op: push left
* -> Const
332:    LDC  0,10(0) 	load const
* <- Const
333:     LD  1,-3(6) 	op: load left
334:    SUB  0,1,0 	op <
335:    JLT  0,2(7) 	br if true
336:    LDC  0,0(0) 	false case
337:    LDA  7,1(7) 	unconditional jmp
338:    LDC  0,1(0) 	true case
* <- Op
* while : jump to end belongs here
* -> compound
* -> Call (output)
* -> Id (x)
340:    LDC  0,0(0) 	id: load varOffset
341:    ADD  0,5,0 	id : calculate the address
342:     ST  0,-3(6) 	id : push base address
* -> Id (i)
343:    LDC  0,-2(0) 	id: load varOffset
344:    ADD  0,6,0 	id : calculate the address
345:     LD  0,0(0) 	load id value
* <- Id (i)
346:     LD  1,-3(6) 	id : pop base address
347:    SUB  0,1,0 	id : calculate element address with index
348:     LD  0,0(0) 	load id value
* <- Id (x)
349:     ST  0,-5(6) 	call : push argument
350:     LD  0,-5(6) 	load arg to ac
351:    OUT  0,0,0 	write ac
* <- Call (output)
* -> assign
* -> Id (i)
352:    LDC  0,-2(0) 	id: load varOffset
353:    ADD  0,6,0 	id : calculate the address
354:    LDA  0,0(0) 	load id address
* <- Id (i)
355:     ST  0,-3(6) 	assign : push left address
* -> Op
* -> Id (i)
356:    LDC  0,-2(0) 	id: load varOffset
357:    ADD  0,6,0 	id : calculate the address
358:     LD  0,0(0) 	load id value
* <- Id (i)
359:     ST  0,-4(6) 	op: push left
* -> Const
360:    LDC  0,1(0) 	load const
* <- Const
361:     LD  1,-4(6) 	op: load left
362:    ADD  0,1,0 	op +
* <- Op
363:     LD  1,-3(6) 	assign : load left address
364:     ST  0,0(1) 	assign : store value
* <- assign
* <- compound
365:    LDA  7,-38(7) 	while : jmp back to test
339:    JEQ  0,26(7) 	while : jmp to end
* <- while
* <- compound
366:     LD  7,-1(6) 	func : load pc with return address
256:    LDA  7,110(7) 	func : unconditional jump to next declaration
* <- Function Declaration (main)
367:    LDC  0,-13(0) 	init: load globalOffset
368:    ADD  6,6,0 	init: initialize mp with globalOffset
369:     ST  6,0(6) 	call: store current mp
370:    LDA  6,0(6) 	call: push new frame
371:    LDA  0,1(7) 	call: save return in ac
372:    LDC  7,257(0) 	jump to main
373:     LD  6,0(6) 	call: pop current frame
* End of execution.
374:   HALT  0,0,0 	
