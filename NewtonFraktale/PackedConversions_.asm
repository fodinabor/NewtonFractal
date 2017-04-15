IFDEF ASMX86_32
.model flat, c
include Structs_.inc
.code

; extern "C" void SseDtoS_(const double* a, float* b);
;
; Description: converts Array of doubles to floats
;
; Requires SSE2

SseDtoS_ proc
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
	mov ecx, [ebp + 12]

	movapd xmm0, xmmword ptr [eax]
	cvtpd2ps xmm1, xmm0
	movaps xmmword ptr [ecx], xmm1

	pop ebp
	ret

SseDtoS_ endp

; extern "C" void AvxDtoS_(const double* a, float* b);
;
; Description: converts Array of doubles to floats
;
; Requires AVX

AvxDtoS_ proc
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
	mov ecx, [ebp + 12]

	vmovapd ymm0, ymmword ptr[eax]
	vcvtpd2ps xmm1, ymm0
	movaps xmmword ptr[ecx], xmm1

	pop ebp
	ret

AvxDtoS_ endp

; extern "C" void SseStoD_(const float* a, double* b);
;
; Description: converts Array of floats to doubles
;
; Requires SSE2

SseStoD_ proc
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
	mov ecx, [ebp + 12]

	movapd xmm0, xmmword ptr [eax]
	cvtps2pd xmm1, xmm0
	movaps xmmword ptr[ecx], xmm1

	pop ebp
	ret

SseStoD_ endp

; extern "C" void AvxStoD_(const float* a, double* b);
;
; Description: converts Array of floats to doubles
;
; Requires AVX

AvxStoD_ proc
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
	mov ecx, [ebp + 12]

	movaps xmm0, xmmword ptr[eax]
	vcvtps2pd ymm1, xmm0
	vmovapd ymmword ptr[ecx], ymm1

	pop ebp
	ret

AvxStoD_ endp

ENDIF

IFDEF ASMX86_64
include Structs_.inc
.code

; extern "C" void SseDtoS_(const double* a, float* b);
;
; Description: converts Array of doubles to floats
;
; Requires SSE2

SseDtoS_ proc
	movapd xmm0, xmmword ptr[rcx]
	cvtpd2ps xmm0, xmm0
	movaps xmmword ptr[rdx], xmm0
	ret

SseDtoS_ endp

; extern "C" void AvxDtoS_(const double* a, float* b);
;
; Description: converts Array of doubles to floats
;
; Requires AVX

AvxDtoS_ proc
	vmovapd ymm0, ymmword ptr[rcx]
	vcvtpd2ps xmm0, ymm0
	movaps xmmword ptr[rdx], xmm0
	ret

AvxDtoS_ endp

; extern "C" void AvxDCtoSC_(const cl_double_complex* a, cl_float_complex* b);
;
; Description: converts cl_double_complex to cl_float_complex
;
; Requires AVX

AvxDCtoSC_ proc

	vmovapd ymm0, ymmword ptr[rcx]
	vcvtpd2ps xmm0, ymm0
	movaps[rdx], xmm0

	mov al, [rcx + cl_double_complex.isPolar]
	mov [rdx + cl_float_complex.isPolar], al

	mov al, [rcx + cl_double_complex.isKarthes]
	mov [rdx + cl_float_complex.isKarthes], al

	ret
AvxDCtoSC_ endp

; extern "C" void SseStoD_(const float* a, double* b);
;
; Description: converts Array of floats to doubles
;
; Requires SSE2

SseStoD_ proc
movaps xmm0, xmmword ptr[rcx]
cvtps2pd xmm0, xmm0
movapd xmmword ptr[rdx], xmm0
ret

SseStoD_ endp

; extern "C" void AvxStoD_(const float* a, double* b);
;
; Description: converts Array of floats to doubles
;
; Requires AVX

AvxStoD_ proc
vmovaps xmm0, xmmword ptr[rcx]
vcvtps2pd ymm1, xmm0
vmovapd ymmword ptr[rdx], ymm1
ret

AvxStoD_ endp

ENDIF

end
