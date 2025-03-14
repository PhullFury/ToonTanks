// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PawnTank.h"
#include "ToonTanks/GameModes/TankGameModeBase.h"

#define OUT

// Sets default values
APawnTank::APawnTank()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void APawnTank::BeginPlay()
{
	Super::BeginPlay();

	PlayerControllerRef = Cast<APlayerController>(GetController());
	GameModeRef = Cast<ATankGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void APawnTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Rotate();
	Move();

	if (PlayerControllerRef)
	{
		FHitResult TraceHitResult;
		PlayerControllerRef->GetHitResultUnderCursor(ECC_Visibility, false, OUT TraceHitResult);
		FVector HitLocation = TraceHitResult.ImpactPoint;
		
		RotateTurret(HitLocation);
	}
}

// Called to bind functionality to input
void APawnTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &APawnTank::CalculateMoveInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawnTank::CalculateRotateInput);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, & APawnTank::Fire);
}

void APawnTank::CalculateMoveInput(float Value)
{
	MoveDirection = FVector(Value * MoveSpeed * GetWorld()->DeltaTimeSeconds, 0, 0);
}

void APawnTank::CalculateRotateInput(float Value)
{
	float RotateAmount = Value * RotateSpeed * GetWorld()->DeltaTimeSeconds;
	FRotator Rotation = FRotator(0, RotateAmount, 0);
	RotationDirection = FQuat(Rotation);
}

void APawnTank::Move()
{
	AddActorLocalOffset(MoveDirection, true);
}

void APawnTank::Rotate()
{
	AddActorLocalRotation(RotationDirection, true);
}

void APawnTank::HandleDestruction()
{
	Super::HandleDestruction();
	//hide player
	bIsPlayerAlive = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void APawnTank::Fire()
{
	if (GameModeRef->GetTargetTurretCount() == 0) {return;}
	Super::Fire();
}

bool APawnTank::GetIsPlayerAlive()
{
	return bIsPlayerAlive;
}